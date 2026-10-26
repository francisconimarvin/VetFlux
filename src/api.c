#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "api.h"
#include "login.h"

#define MAX_SESSIONS 128
#define TOKEN_SIZE 65

typedef struct api_session {
    char token[TOKEN_SIZE];
    char rut[16];
} ApiSession;

static ApiSession sessions[MAX_SESSIONS];

static void replyError(struct mg_connection *c, int status, const char *message) {
    mg_http_reply(c, status, "Content-Type: application/json\r\n",
                  "{\"error\":\"%s\"}\n", message);
}

static void jsonString(struct mg_connection *c, const char *value) {
    mg_printf(c, "\"");
    for (const unsigned char *p = (const unsigned char *) value; *p; p++) {
        if (*p == '"' || *p == '\\') mg_printf(c, "\\%c", *p);
        else if (*p < 0x20) mg_printf(c, " ");
        else mg_printf(c, "%c", *p);
    }
    mg_printf(c, "\"");
}

static void replyResult(struct mg_connection *c, int status, PGresult *res) {
    int rows = PQntuples(res), cols = PQnfields(res);
    size_t size = 3;
    for (int i = 0; i < cols; i++) size += strlen(PQfname(res, i)) * 6 + 8;
    for (int i = 0; i < rows; i++) for (int j = 0; j < cols; j++)
        size += (PQgetisnull(res, i, j) ? 4 : strlen(PQgetvalue(res, i, j)) * 6 + 2) + 4;
    char *body = calloc(size, 1);
    if (body == NULL) { replyError(c, 500, "Sin memoria"); return; }
    size_t ofs = 0;
    body[ofs++] = '[';
    for (int i = 0; i < rows; i++) {
        ofs += (size_t) snprintf(body + ofs, size - ofs, "%s{", i ? "," : "");
        for (int j = 0; j < cols; j++) {
            const char *key = PQfname(res, j), *value = PQgetvalue(res, i, j);
            ofs += (size_t) snprintf(body + ofs, size - ofs, "%s\"", j ? "," : "");
            for (const unsigned char *p = (const unsigned char *) key; *p; p++) {
                if (*p == '"' || *p == '\\') body[ofs++] = '\\';
                body[ofs++] = *p < 0x20 ? ' ' : (char) *p;
            }
            body[ofs++] = '"'; body[ofs++] = ':';
            if (PQgetisnull(res, i, j)) { memcpy(body + ofs, "null", 4); ofs += 4; }
            else {
                body[ofs++] = '"';
                for (const unsigned char *p = (const unsigned char *) value; *p; p++) {
                    if (*p == '"' || *p == '\\') body[ofs++] = '\\';
                    body[ofs++] = *p < 0x20 ? ' ' : (char) *p;
                }
                body[ofs++] = '"';
            }
        }
        body[ofs++] = '}';
    }
    body[ofs++] = ']'; body[ofs++] = '\n'; body[ofs] = '\0';
    mg_http_reply(c, status, "Content-Type: application/json\r\n", "%s", body);
    free(body);
}

static void replyCommand(struct mg_connection *c, int status, PGresult *res) {
    mg_http_reply(c, status, "Content-Type: application/json\r\n",
                  "{\"updated\":%s}\n",
                  strcmp(PQcmdTuples(res), "0") == 0 ? "false" : "true");
}

static char *jsonField(struct mg_http_message *hm, const char *field) {
    char path[80];
    snprintf(path, sizeof(path), "$.%s", field);
    return mg_json_get_str(hm->body, path);
}

static ApiSession *getSession(struct mg_http_message *hm) {
    struct mg_str *header = mg_http_get_header(hm, "Authorization");
    if (header == NULL || header->len < 8 || memcmp(header->buf, "Bearer ", 7) != 0)
        return NULL;
    for (int i = 0; i < MAX_SESSIONS; i++)
        if (sessions[i].token[0] && strlen(sessions[i].token) == header->len - 7 &&
            memcmp(sessions[i].token, header->buf + 7, header->len - 7) == 0)
            return &sessions[i];
    return NULL;
}

static void loginRequest(struct mg_connection *c, struct mg_http_message *hm,
                         PGconn *conn) {
    char *rut = jsonField(hm, "rut"), *password = jsonField(hm, "password");
    if (rut == NULL || password == NULL) {
        mg_free(rut); mg_free(password);
        replyError(c, 400, "Se requieren rut y password");
        return;
    }
    Sesion *auth = login(conn, rut, password);
    mg_free(password);
    if (auth == NULL) {
        mg_free(rut);
        replyError(c, 401, "Credenciales invalidas");
        return;
    }
    ApiSession *slot = NULL;
    for (int i = 0; i < MAX_SESSIONS; i++) if (!sessions[i].token[0]) { slot = &sessions[i]; break; }
    if (slot == NULL || mg_random_str(slot->token, sizeof(slot->token)) == NULL) {
        free(auth); mg_free(rut);
        replyError(c, 503, "No se pudo crear la sesion");
        return;
    }
    snprintf(slot->rut, sizeof(slot->rut), "%s", auth->rut);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                  "{\"token\":\"%s\",\"token_type\":\"Bearer\"}\n", slot->token);
    free(auth);
    mg_free(rut);
}

static bool numberId(const char *s) {
    if (s == NULL || *s == '\0') return false;
    for (; *s; s++) if (!isdigit((unsigned char) *s)) return false;
    return true;
}

static void resourceRequest(struct mg_connection *c, struct mg_http_message *hm,
                            PGconn *conn, ApiSession *session) {
    char path[256];
    snprintf(path, sizeof(path), "%.*s", (int) hm->uri.len, hm->uri.buf);
    char *query = strchr(path, '?'); if (query) *query = '\0';
    char *parts[4] = {0}; int count = 0;
    for (char *p = strtok(path, "/"); p && count < 4; p = strtok(NULL, "/")) parts[count++] = p;
    if (count < 2 || strcmp(parts[0], "api") != 0) { replyError(c, 404, "Ruta no encontrada"); return; }
    const char *resource = parts[1], *id = count > 2 ? parts[2] : NULL;
    bool get = mg_strcmp(hm->method, mg_str("GET")) == 0;
    bool post = mg_strcmp(hm->method, mg_str("POST")) == 0;
    bool put = mg_strcmp(hm->method, mg_str("PUT")) == 0;
    bool del = mg_strcmp(hm->method, mg_str("DELETE")) == 0;
    const char *values[8] = {0}; int n = 0;
    char *owned[8] = {0};
    char filter[32];
    const char *sql = NULL;

    if (strcmp(resource, "tutores") == 0) {
        if (get && id) { values[n++] = id; values[n++] = session->rut; sql = "SELECT * FROM public.tutor WHERE rut=$1 AND doctor_fk=$2"; }
        else if (get) { values[n++] = session->rut; sql = "SELECT * FROM public.tutor WHERE doctor_fk=$1 ORDER BY rut"; }
        else if (post) { const char *fields[]={"rut","nombre","email","telefono","domicilio"}; for(int i=0;i<5;i++) values[n++]=owned[i]=jsonField(hm,fields[i]); values[n++]=session->rut; sql="INSERT INTO public.tutor(rut,nombre,email,telefono,domicilio,doctor_fk) VALUES($1,$2,$3,$4,$5,$6) RETURNING *"; }
        else if (put && id) { const char *fieldNames[]={"nombre","email","telefono","domicilio"}; const char *queries[]={"UPDATE public.tutor SET nombre=$1 WHERE rut=$2 AND doctor_fk=$3 RETURNING *","UPDATE public.tutor SET email=$1 WHERE rut=$2 AND doctor_fk=$3 RETURNING *","UPDATE public.tutor SET telefono=$1 WHERE rut=$2 AND doctor_fk=$3 RETURNING *","UPDATE public.tutor SET domicilio=$1 WHERE rut=$2 AND doctor_fk=$3 RETURNING *"}; int found=0; for(int i=0;i<4;i++) if ((owned[0]=jsonField(hm,fieldNames[i]))!=NULL) { sql=queries[i]; values[n++]=owned[0]; values[n++]=id; values[n++]=session->rut; found=1; break; } if(!found) { replyError(c,400,"Campo de actualizacion requerido"); return; } }
        else if (del && id) { values[n++]=id; values[n++]=session->rut; sql="DELETE FROM public.tutor WHERE rut=$1 AND doctor_fk=$2 RETURNING *"; }
    } else if (strcmp(resource,"pacientes")==0) {
        if(get&&id){values[n++]=id;values[n++]=session->rut;sql="SELECT * FROM public.paciente WHERE id=$1::integer AND doctor_fk=$2";}
        else if(get){if(mg_http_get_var(&hm->query,"tutor_fk",filter,sizeof(filter))>=0){values[n++]=filter;values[n++]=session->rut;sql="SELECT * FROM public.paciente WHERE tutor_fk=$1 AND doctor_fk=$2 ORDER BY id";}else{values[n++]=session->rut;sql="SELECT * FROM public.paciente WHERE doctor_fk=$1 ORDER BY id";}}
        else if(post){const char *f[]={"nombre","edad","sexo","especie","raza","color","tutor_fk"};for(int i=0;i<7;i++)values[n++]=owned[i]=jsonField(hm,f[i]);values[n++]=session->rut;sql="INSERT INTO public.paciente(nombre,edad,sexo,especie,raza,color,tutor_fk,doctor_fk) VALUES($1,NULLIF($2,'')::integer,NULLIF($3,''),$4,NULLIF($5,''),NULLIF($6,''),NULLIF($7,''),$8) RETURNING *";}
        else if((put||del)&&id&&numberId(id)){if(del){values[n++]=id;values[n++]=session->rut;sql="DELETE FROM public.paciente WHERE id=$1::integer AND doctor_fk=$2 RETURNING *";}else{owned[0]=jsonField(hm,"nombre");owned[1]=jsonField(hm,"edad");owned[2]=jsonField(hm,"sexo");owned[3]=jsonField(hm,"especie");owned[4]=jsonField(hm,"raza");owned[5]=jsonField(hm,"color");owned[6]=jsonField(hm,"tutor_fk");values[n++]=owned[0]?owned[0]:"";values[n++]=owned[1]?owned[1]:"";values[n++]=owned[2]?owned[2]:"";values[n++]=owned[3]?owned[3]:"";values[n++]=owned[4]?owned[4]:"";values[n++]=owned[5]?owned[5]:"";values[n++]=owned[6]?owned[6]:"";values[n++]=id;values[n++]=session->rut;sql="UPDATE public.paciente SET nombre=COALESCE(NULLIF($1,''),nombre),edad=COALESCE(NULLIF($2,'')::integer,edad),sexo=COALESCE(NULLIF($3,''),sexo),especie=COALESCE(NULLIF($4,''),especie),raza=COALESCE(NULLIF($5,''),raza),color=COALESCE(NULLIF($6,''),color),tutor_fk=COALESCE(NULLIF($7,''),tutor_fk) WHERE id=$8::integer AND doctor_fk=$9 RETURNING *";}}
    } else if (strcmp(resource,"agenda")==0) {
        if(get&&id&&numberId(id)){values[n++]=id;values[n++]=session->rut;sql="SELECT a.* FROM public.agenda a JOIN public.paciente p ON p.id=a.id_paciente_fk WHERE a.id=$1::integer AND p.doctor_fk=$2";}
        else if(get){if(mg_http_get_var(&hm->query,"id_paciente",filter,sizeof(filter))>=0&&numberId(filter)){values[n++]=filter;values[n++]=session->rut;sql="SELECT a.* FROM public.agenda a JOIN public.paciente p ON p.id=a.id_paciente_fk WHERE p.id=$1::integer AND p.doctor_fk=$2 ORDER BY a.fecha_hora";}else{values[n++]=session->rut;sql="SELECT a.* FROM public.agenda a JOIN public.paciente p ON p.id=a.id_paciente_fk WHERE p.doctor_fk=$1 ORDER BY a.fecha_hora";}}
        else if(post){owned[0]=jsonField(hm,"fecha_hora");owned[1]=jsonField(hm,"notas");owned[2]=jsonField(hm,"id_paciente");values[n++]=owned[0];values[n++]=owned[1]?owned[1]:"";values[n++]=owned[2];values[n++]=session->rut;sql="INSERT INTO public.agenda(fecha_hora,notas,id_paciente_fk) SELECT $1::timestamptz,NULLIF($2,''),p.id FROM public.paciente p WHERE p.id=$3::integer AND p.doctor_fk=$4 RETURNING *";}
        else if(put&&id&&numberId(id)){owned[0]=jsonField(hm,"fecha_hora");owned[1]=jsonField(hm,"notas");if(owned[0]){values[n++]=owned[0];values[n++]=id;values[n++]=session->rut;sql="UPDATE public.agenda a SET fecha_hora=$1::timestamptz FROM public.paciente p WHERE a.id=$2::integer AND p.id=a.id_paciente_fk AND p.doctor_fk=$3 RETURNING a.*";}else if(owned[1]){values[n++]=owned[1];values[n++]=id;values[n++]=session->rut;sql="UPDATE public.agenda a SET notas=NULLIF($1,'') FROM public.paciente p WHERE a.id=$2::integer AND p.id=a.id_paciente_fk AND p.doctor_fk=$3 RETURNING a.*";}}
        else if(del&&id&&numberId(id)){values[n++]=id;values[n++]=session->rut;sql="DELETE FROM public.agenda a USING public.paciente p WHERE a.id=$1::integer AND p.id=a.id_paciente_fk AND p.doctor_fk=$2 RETURNING a.*";}
    } else if(strcmp(resource,"fichas-medicas")==0){
        if(get&&id){values[n++]=id;values[n++]=session->rut;sql="SELECT f.* FROM public.ficha_medica f JOIN public.paciente p ON p.id=f.id_paciente_fk WHERE f.id=$1::integer AND p.doctor_fk=$2";}
        else if(get){values[n++]=session->rut;sql="SELECT f.* FROM public.ficha_medica f JOIN public.paciente p ON p.id=f.id_paciente_fk WHERE p.doctor_fk=$1 ORDER BY f.id";}
        else if(post){owned[0]=jsonField(hm,"titulo");owned[1]=jsonField(hm,"descripcion");owned[2]=jsonField(hm,"id_paciente");values[n++]=owned[0];values[n++]=owned[1];values[n++]=owned[2];values[n++]=session->rut;sql="INSERT INTO public.ficha_medica(titulo,descripcion,id_paciente_fk) SELECT $1,$2,p.id FROM public.paciente p WHERE p.id=$3::integer AND p.doctor_fk=$4 RETURNING *";}
        else if(put&&id){owned[0]=jsonField(hm,"titulo");owned[1]=jsonField(hm,"descripcion");values[n++]=owned[0];values[n++]=owned[1];values[n++]=id;values[n++]=session->rut;sql="UPDATE public.ficha_medica f SET titulo=COALESCE($1,titulo),descripcion=COALESCE($2,descripcion) FROM public.paciente p WHERE f.id=$3::integer AND p.id=f.id_paciente_fk AND p.doctor_fk=$4 RETURNING f.*";}
        else if(del&&id){values[n++]=id;values[n++]=session->rut;sql="DELETE FROM public.ficha_medica f USING public.paciente p WHERE f.id=$1::integer AND p.id=f.id_paciente_fk AND p.doctor_fk=$2 RETURNING f.*";}
    } else if(strcmp(resource,"recetas")==0){
        if(get&&id){values[n++]=id;values[n++]=session->rut;sql="SELECT r.* FROM public.receta r JOIN public.paciente p ON p.id=r.paciente_fk WHERE r.id=$1::integer AND p.doctor_fk=$2";}
        else if(get){if(mg_http_get_var(&hm->query,"id_paciente",filter,sizeof(filter))>=0&&numberId(filter)){values[n++]=filter;values[n++]=session->rut;sql="SELECT r.* FROM public.receta r JOIN public.paciente p ON p.id=r.paciente_fk WHERE p.id=$1::integer AND p.doctor_fk=$2 ORDER BY r.id";}else{values[n++]=session->rut;sql="SELECT r.* FROM public.receta r JOIN public.paciente p ON p.id=r.paciente_fk WHERE p.doctor_fk=$1 ORDER BY r.id";}}
        else if(post){owned[0]=jsonField(hm,"dosis");owned[1]=jsonField(hm,"id_paciente");owned[2]=jsonField(hm,"id_medicamento");values[n++]=owned[0];values[n++]=owned[1];values[n++]=owned[2];values[n++]=session->rut;sql="INSERT INTO public.receta(dosis,paciente_fk,medicamento_fk) SELECT $1,p.id,m.id FROM public.paciente p CROSS JOIN public.medicamentos m WHERE p.id=$2::integer AND m.id=$3::integer AND p.doctor_fk=$4 RETURNING *";}
        else if(put&&id){owned[0]=jsonField(hm,"dosis");values[n++]=owned[0];values[n++]=id;values[n++]=session->rut;sql="UPDATE public.receta r SET dosis=$1 FROM public.paciente p WHERE r.id=$2::integer AND p.id=r.paciente_fk AND p.doctor_fk=$3 RETURNING r.*";}
    } else if(strcmp(resource,"inventario")==0){
        if(get){values[n++]=session->rut;sql="SELECT i.id AS instrumento_id,i.nombre,inv.cantidad FROM public.inventario inv JOIN public.instrumentos i ON i.id=inv.instrumento_fk WHERE inv.doctor_fk=$1 ORDER BY i.nombre";}
        else if(put&&id&&numberId(id)){owned[0]=jsonField(hm,"cantidad");values[n++]=owned[0];values[n++]=id;values[n++]=session->rut;sql="UPDATE public.inventario SET cantidad=$1::integer WHERE instrumento_fk=$2::integer AND doctor_fk=$3 RETURNING *";}
    }

    if (sql == NULL) { for(int i=0;i<8;i++) mg_free(owned[i]); replyError(c, 404, "Ruta o metodo no soportado"); return; }
    PGresult *res = PQexecParams(conn, sql, n, NULL, values, NULL, NULL, 0);
    for(int i=0;i<8;i++) mg_free(owned[i]);
    if (res == NULL || (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK)) {
        if(res) PQclear(res);
        replyError(c, 400, "La consulta no pudo completarse");
        return;
    }
    if(PQresultStatus(res)==PGRES_TUPLES_OK) replyResult(c, (post?201:200), res);
    else replyCommand(c, 200, res);
    PQclear(res);
}

void apiEventHandler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev != MG_EV_HTTP_MSG) return;
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    ApiContext *ctx = (ApiContext *) c->fn_data;
    if (ctx == NULL || ctx->conn == NULL || PQstatus(ctx->conn) != CONNECTION_OK) {
        replyError(c, 503, "Base de datos no disponible"); return;
    }
    if (mg_strcmp(hm->uri, mg_str("/api/auth/login")) == 0 && mg_strcmp(hm->method, mg_str("POST")) == 0) {
        loginRequest(c, hm, ctx->conn); return;
    }
    ApiSession *session = getSession(hm);
    if (session == NULL) { replyError(c, 401, "Autenticacion requerida"); return; }
    if (mg_strcmp(hm->uri, mg_str("/api/auth/logout")) == 0 && mg_strcmp(hm->method, mg_str("POST")) == 0) {
        memset(session, 0, sizeof(*session));
        mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"logged_out\":true}\n");
        return;
    }
    resourceRequest(c, hm, ctx->conn, session);
}
