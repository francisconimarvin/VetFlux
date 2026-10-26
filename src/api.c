#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "api.h"
#include "login.h"
#include "tutor.h"
#include "paciente.h"
#include "agenda.h"
#include "inventario.h"
#include "fichaMedica.h"
#include "receta.h"

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

static bool appendBytes(char **body, size_t *length, size_t *capacity,
                        const char *value, size_t valueLength) {
    if (valueLength > (size_t) -1 - *length - 1) return false;
    size_t needed = *length + valueLength + 1;
    if (needed > *capacity) {
        size_t newCapacity = *capacity == 0 ? 256 : *capacity;
        while (newCapacity < needed) {
            if (newCapacity > (size_t) -1 / 2) { newCapacity = needed; break; }
            newCapacity *= 2;
        }
        char *newBody = realloc(*body, newCapacity);
        if (newBody == NULL) return false;
        *body = newBody;
        *capacity = newCapacity;
    }
    memcpy(*body + *length, value, valueLength);
    *length += valueLength;
    (*body)[*length] = '\0';
    return true;
}

static bool appendText(char **body, size_t *length, size_t *capacity,
                       const char *value) {
    return appendBytes(body, length, capacity, value, strlen(value));
}

static bool appendJsonString(char **body, size_t *length, size_t *capacity,
                             const char *value) {
    if (!appendText(body, length, capacity, "\"")) return false;
    for (const unsigned char *p = (const unsigned char *) value; *p; p++) {
        char escaped[2];
        size_t escapedLength = 1;
        if (*p == '"' || *p == '\\') {
            escaped[0] = '\\'; escaped[1] = (char) *p; escapedLength = 2;
        } else escaped[0] = *p < 0x20 ? ' ' : (char) *p;
        if (!appendBytes(body, length, capacity, escaped, escapedLength)) return false;
    }
    return appendText(body, length, capacity, "\"");
}

static void replyResult(struct mg_connection *c, int status, PGresult *res) {
    char *body = NULL;
    size_t length = 0, capacity = 0;
    bool success = appendText(&body, &length, &capacity, "[");
    for (int i = 0; success && i < PQntuples(res); i++) {
        if (i) success = appendText(&body, &length, &capacity, ",");
        if (success) success = appendText(&body, &length, &capacity, "{");
        for (int j = 0; success && j < PQnfields(res); j++) {
            if (j) success = appendText(&body, &length, &capacity, ",");
            if (success) success = appendJsonString(&body, &length, &capacity, PQfname(res, j));
            if (success) success = appendText(&body, &length, &capacity, ":");
            if (success) {
                if (PQgetisnull(res, i, j)) success = appendText(&body, &length, &capacity, "null");
                else success = appendJsonString(&body, &length, &capacity, PQgetvalue(res, i, j));
            }
        }
        if (success) success = appendText(&body, &length, &capacity, "}");
    }
    if (success) success = appendText(&body, &length, &capacity, "]\n");
    if (success) mg_http_reply(c, status, "Content-Type: application/json\r\n", "%s", body);
    else replyError(c, 500, "No se pudo construir la respuesta");
    free(body);
}

static char *jsonField(struct mg_http_message *hm, const char *field) {
    char path[80];
    snprintf(path, sizeof(path), "$.%s", field);
    char *value = mg_json_get_str(hm->body, path);
    if (value != NULL) return value;
    struct mg_str token = mg_json_get_tok(hm->body, path);
    if (token.buf == NULL || !mg_json_get_num(hm->body, path, NULL)) return NULL;
    value = malloc(token.len + 1);
    if (value == NULL) return NULL;
    memcpy(value, token.buf, token.len);
    value[token.len] = '\0';
    return value;
}

static bool copyField(struct mg_http_message *hm, const char *field,
                      char *destination, size_t capacity) {
    char path[80];
    snprintf(path, sizeof(path), "$.%s", field);
    if (mg_json_get(hm->body, path, NULL) == MG_JSON_NOT_FOUND) return true;
    char *value = jsonField(hm, field);
    if (value == NULL) return false;
    size_t length = strlen(value);
    bool fits = length < capacity;
    if (fits) memcpy(destination, value, length + 1);
    mg_free(value);
    return fits;
}

static bool hasField(struct mg_http_message *hm, const char *field) {
    char path[80];
    snprintf(path, sizeof(path), "$.%s", field);
    return mg_json_get(hm->body, path, NULL) >= 0;
}

static ApiSession *getSession(struct mg_http_message *hm) {
    struct mg_str *header = mg_http_get_header(hm, "Authorization");
    if (header == NULL || header->len < 8 || memcmp(header->buf, "Bearer ", 7) != 0) return NULL;
    for (int i = 0; i < MAX_SESSIONS; i++)
        if (sessions[i].token[0] && strlen(sessions[i].token) == header->len - 7 &&
            memcmp(sessions[i].token, header->buf + 7, header->len - 7) == 0)
            return &sessions[i];
    return NULL;
}

static void handleLogin(struct mg_connection *c, struct mg_http_message *hm,
                        PGconn *conn) {
    char *rut = jsonField(hm, "rut"), *password = jsonField(hm, "password");
    if (rut == NULL || password == NULL || strlen(rut) >= sizeof(((Sesion *) 0)->rut) ||
        strlen(password) >= sizeof(((Sesion *) 0)->psswd)) {
        mg_free(rut); mg_free(password);
        replyError(c, 400, "Credenciales faltantes o demasiado largas");
        return;
    }
    Sesion *auth = login(conn, rut, password);
    mg_free(password);
    if (auth == NULL) {
        mg_free(rut); replyError(c, 401, "Credenciales invalidas"); return;
    }
    ApiSession *slot = NULL;
    for (int i = 0; i < MAX_SESSIONS; i++) if (!sessions[i].token[0]) { slot = &sessions[i]; break; }
    if (slot == NULL || mg_random_str(slot->token, sizeof(slot->token)) == NULL) {
        free(auth); mg_free(rut); replyError(c, 503, "No se pudo crear la sesion"); return;
    }
    snprintf(slot->rut, sizeof(slot->rut), "%s", auth->rut);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                  "{\"token\":\"%s\",\"token_type\":\"Bearer\"}\n", slot->token);
    free(auth); mg_free(rut);
}

static bool numberId(const char *value) {
    if (value == NULL || *value == '\0') return false;
    for (const unsigned char *p = (const unsigned char *) value; *p; p++)
        if (!isdigit(*p)) return false;
    return true;
}

static PGresult *handleTutores(struct mg_http_message *hm, PGconn *conn,
                               const Sesion *sesion, const char *id,
                               bool *created) {
    bool get=mg_strcmp(hm->method,mg_str("GET"))==0;
    bool post=mg_strcmp(hm->method,mg_str("POST"))==0;
    bool put=mg_strcmp(hm->method,mg_str("PUT"))==0;
    bool del=mg_strcmp(hm->method,mg_str("DELETE"))==0;
    if(get) return id?tutorGet(conn,sesion,id):tutorList(conn,sesion);
    if(del&&id) return tutorDelete(conn,sesion,id);
    if(!post&&!(put&&id)) return NULL;
    Tutor data={0};
    if(!copyField(hm,"rut",data.rut,sizeof(data.rut))||
       !copyField(hm,"nombre",data.nombre,sizeof(data.nombre))||
       !copyField(hm,"email",data.email,sizeof(data.email))||
       !copyField(hm,"telefono",data.telefono,sizeof(data.telefono))||
       !copyField(hm,"domicilio",data.domicilio,sizeof(data.domicilio))) return NULL;
    if(post){*created=true;return tutorCreate(conn,sesion,&data);}
    if(!hasField(hm,"nombre")&&!hasField(hm,"email")&&
       !hasField(hm,"telefono")&&!hasField(hm,"domicilio")) return NULL;
    return tutorUpdate(conn,sesion,id,&data);
}

static PGresult *handlePacientes(struct mg_http_message *hm, PGconn *conn,
                                 const Sesion *sesion, const char *id,
                                 bool *created) {
    bool get=mg_strcmp(hm->method,mg_str("GET"))==0;
    bool post=mg_strcmp(hm->method,mg_str("POST"))==0;
    bool put=mg_strcmp(hm->method,mg_str("PUT"))==0;
    bool del=mg_strcmp(hm->method,mg_str("DELETE"))==0;
    char tutorRut[32];
    if(get&&id) return numberId(id)?pacienteGet(conn,sesion,id):NULL;
    if(get&&mg_http_get_var(&hm->query,"tutor_fk",tutorRut,sizeof(tutorRut))>=0)
        return pacienteListByTutor(conn,sesion,tutorRut);
    if(get) return pacienteList(conn,sesion);
    if(del&&id&&numberId(id)) return pacienteDelete(conn,sesion,id);
    if(!post&&!(put&&id&&numberId(id))) return NULL;
    Paciente data={0};
    if(!copyField(hm,"nombre",data.nombre,50)||
       !copyField(hm,"edad",data.edad,sizeof(data.edad))||
       !copyField(hm,"sexo",data.sexo,2)||
       !copyField(hm,"especie",data.especie,20)||
       !copyField(hm,"raza",data.raza,20)||
       !copyField(hm,"color",data.color,30)||
       !copyField(hm,"tutor_fk",data.tutor_fk,sizeof(data.tutor_fk))) return NULL;
    if(post){*created=true;return pacienteCreate(conn,sesion,&data);}
    return pacienteUpdate(conn,sesion,id,&data);
}

static PGresult *handleAgenda(struct mg_http_message *hm, PGconn *conn,
                              const Sesion *sesion, const char *id,
                              bool *created) {
    bool get=mg_strcmp(hm->method,mg_str("GET"))==0;
    bool post=mg_strcmp(hm->method,mg_str("POST"))==0;
    bool put=mg_strcmp(hm->method,mg_str("PUT"))==0;
    bool del=mg_strcmp(hm->method,mg_str("DELETE"))==0;
    char pacienteId[32];
    if(get&&id) return numberId(id)?agendaGet(conn,sesion,id):NULL;
    if(get&&mg_http_get_var(&hm->query,"id_paciente",pacienteId,sizeof(pacienteId))>=0)
        return numberId(pacienteId)?agendaListByPaciente(conn,sesion,pacienteId):NULL;
    if(get) return agendaList(conn,sesion);
    if(del&&id&&numberId(id)) return agendaDelete(conn,sesion,id);
    if(!post&&!(put&&id&&numberId(id))) return NULL;
    Agenda data={0};
    bool hasFecha=hasField(hm,"fecha_hora"), hasNotas=hasField(hm,"notas");
    if(!copyField(hm,"fecha_hora",data.fechaHora,sizeof(data.fechaHora))||
       !copyField(hm,"notas",data.notas,sizeof(data.notas))||
       !copyField(hm,"id_paciente",data.idPaciente,sizeof(data.idPaciente))) return NULL;
    if(post){*created=true;return agendaCreate(conn,sesion,&data);}
    if(!hasFecha&&!hasNotas) return NULL;
    return agendaUpdate(conn,sesion,id,&data,hasFecha,hasNotas);
}

static PGresult *handleFichasMedicas(struct mg_http_message *hm, PGconn *conn,
                                     const Sesion *sesion, const char *id,
                                     bool *created) {
    bool get=mg_strcmp(hm->method,mg_str("GET"))==0;
    bool post=mg_strcmp(hm->method,mg_str("POST"))==0;
    bool put=mg_strcmp(hm->method,mg_str("PUT"))==0;
    bool del=mg_strcmp(hm->method,mg_str("DELETE"))==0;
    if(get&&id) return numberId(id)?fichaMedicaGet(conn,sesion,id):NULL;
    if(get) return fichaMedicaList(conn,sesion);
    if(del&&id&&numberId(id)) return fichaMedicaDelete(conn,sesion,id);
    if(!post&&!(put&&id&&numberId(id))) return NULL;
    FichaMedica data={0};
    bool hasTitulo=hasField(hm,"titulo"), hasDescripcion=hasField(hm,"descripcion");
    if(!copyField(hm,"titulo",data.titulo,sizeof(data.titulo))||
       !copyField(hm,"descripcion",data.descripcion,sizeof(data.descripcion))||
       !copyField(hm,"id_paciente",data.idPaciente,sizeof(data.idPaciente))) return NULL;
    if(post){*created=true;return fichaMedicaCreate(conn,sesion,&data);}
    if(!hasTitulo&&!hasDescripcion) return NULL;
    return fichaMedicaUpdate(conn,sesion,id,&data,hasTitulo,hasDescripcion);
}

static PGresult *handleRecetas(struct mg_http_message *hm, PGconn *conn,
                               const Sesion *sesion, const char *id,
                               bool *created) {
    bool get=mg_strcmp(hm->method,mg_str("GET"))==0;
    bool post=mg_strcmp(hm->method,mg_str("POST"))==0;
    bool put=mg_strcmp(hm->method,mg_str("PUT"))==0;
    char pacienteId[32];
    if(get&&id) return numberId(id)?recetaGet(conn,sesion,id):NULL;
    if(get&&mg_http_get_var(&hm->query,"id_paciente",pacienteId,sizeof(pacienteId))>=0)
        return numberId(pacienteId)?recetaListByPaciente(conn,sesion,pacienteId):NULL;
    if(get) return recetaList(conn,sesion);
    if(!post&&!(put&&id&&numberId(id))) return NULL;
    Receta data={0};
    if(!copyField(hm,"dosis",data.dosis,sizeof(data.dosis))||
       !copyField(hm,"id_paciente",data.idPaciente,sizeof(data.idPaciente))||
       !copyField(hm,"id_medicamento",data.idMedicamnento,sizeof(data.idMedicamnento))) return NULL;
    if(post){*created=true;return recetaCreate(conn,sesion,&data);}
    if(!hasField(hm,"dosis")) return NULL;
    return recetaUpdate(conn,sesion,id,&data);
}

static PGresult *handleInventario(struct mg_http_message *hm, PGconn *conn,
                                  const Sesion *sesion, const char *id,
                                  bool *created) {
    (void) created;
    bool get=mg_strcmp(hm->method,mg_str("GET"))==0;
    bool put=mg_strcmp(hm->method,mg_str("PUT"))==0;
    if(get) return inventarioList(conn,sesion);
    if(!put||!id||!numberId(id)) return NULL;
    Inventario data={0};
    if(!copyField(hm,"cantidad",data.cantidad,sizeof(data.cantidad))) return NULL;
    return inventarioUpdate(conn,sesion,id,&data);
}

static void handleLogout(struct mg_connection *c, ApiSession *session) {
    memset(session,0,sizeof(*session));
    mg_http_reply(c,200,"Content-Type: application/json\r\n","{\"logged_out\":true}\n");
}

typedef PGresult *(*ResourceHandler)(struct mg_http_message *, PGconn *,
    const Sesion *, const char *, bool *);

typedef struct resource_route {
    const char *name;
    ResourceHandler handler;
} ResourceRoute;

static const ResourceRoute resourceRoutes[] = {
    {"tutores", handleTutores},
    {"pacientes", handlePacientes},
    {"agenda", handleAgenda},
    {"fichas-medicas", handleFichasMedicas},
    {"recetas", handleRecetas},
    {"inventario", handleInventario}
};

static void resourceRequest(struct mg_connection *c, struct mg_http_message *hm,
                            PGconn *conn, ApiSession *apiSession) {
    char path[256];
    snprintf(path,sizeof(path),"%.*s",(int)hm->uri.len,hm->uri.buf);
    char *query=strchr(path,'?'); if(query)*query='\0';
    char *parts[4]={0}; int count=0;
    for(char *part=strtok(path,"/");part&&count<4;part=strtok(NULL,"/"))parts[count++]=part;
    if(count<2||count>3||strcmp(parts[0],"api")!=0){replyError(c,404,"Ruta no encontrada");return;}

    Sesion sesion={0};
    snprintf(sesion.rut,sizeof(sesion.rut),"%s",apiSession->rut);
    const char *id=count>2?parts[2]:NULL;
    bool created=false;
    PGresult *res=NULL;
    bool knownRoute=false;
    for(size_t i=0;i<sizeof(resourceRoutes)/sizeof(resourceRoutes[0]);i++) {
        if(strcmp(parts[1],resourceRoutes[i].name)==0) {
            knownRoute=true;
            res=resourceRoutes[i].handler(hm,conn,&sesion,id,&created);
            break;
        }
    }
    if(!knownRoute){replyError(c,404,"Ruta no encontrada");return;}
    if(res==NULL){replyError(c,400,"Ruta, metodo o datos invalidos");return;}
    if(PQresultStatus(res)!=PGRES_TUPLES_OK){PQclear(res);replyError(c,400,"La operacion no pudo completarse");return;}
    if(PQntuples(res)==0 && (created || mg_strcmp(hm->method,mg_str("PUT"))==0 ||
       mg_strcmp(hm->method,mg_str("DELETE"))==0)){
        PQclear(res);replyError(c,404,"Registro inexistente o sin acceso");return;
    }
    replyResult(c,created?201:200,res);
    PQclear(res);
}

void apiEventHandler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev != MG_EV_HTTP_MSG) return;
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    ApiContext *ctx = (ApiContext *) c->fn_data;
    if (ctx == NULL || ctx->conn == NULL || PQstatus(ctx->conn) != CONNECTION_OK) {
        replyError(c,503,"Base de datos no disponible");return;
    }
    if (mg_strcmp(hm->uri,mg_str("/api/auth/login"))==0 && mg_strcmp(hm->method,mg_str("POST"))==0) {
        handleLogin(c,hm,ctx->conn);return;
    }
    ApiSession *session=getSession(hm);
    if(session==NULL){replyError(c,401,"Autenticacion requerida");return;}
    if(mg_strcmp(hm->uri,mg_str("/api/auth/logout"))==0 && mg_strcmp(hm->method,mg_str("POST"))==0){
        handleLogout(c,session);return;
    }
    resourceRequest(c,hm,ctx->conn,session);
}
