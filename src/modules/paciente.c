#include <libpq-fe.h>
#include "paciente.h"

PGresult *pacienteList(PGconn *conn, const Sesion *sesion) {
    const char *values[] = {sesion->rut};
    return PQexecParams(conn,
        "SELECT * FROM public.paciente WHERE doctor_fk=$1 ORDER BY id",
        1, NULL, values, NULL, NULL, 0);
}

PGresult *pacienteGet(PGconn *conn, const Sesion *sesion, const char *id) {
    const char *values[] = {id, sesion->rut};
    return PQexecParams(conn,
        "SELECT * FROM public.paciente WHERE id=$1::integer AND doctor_fk=$2",
        2, NULL, values, NULL, NULL, 0);
}

PGresult *pacienteListByTutor(PGconn *conn, const Sesion *sesion,
                              const char *tutorRut) {
    const char *values[] = {tutorRut, sesion->rut};
    return PQexecParams(conn,
        "SELECT * FROM public.paciente WHERE tutor_fk=$1 AND doctor_fk=$2 ORDER BY id",
        2, NULL, values, NULL, NULL, 0);
}

PGresult *pacienteCreate(PGconn *conn, const Sesion *sesion,
                         const Paciente *paciente) {
    const char *values[] = {paciente->nombre, paciente->edad, paciente->sexo,
        paciente->especie, paciente->raza, paciente->color, paciente->tutor_fk,
        sesion->rut};
    return PQexecParams(conn,
        "INSERT INTO public.paciente(nombre,edad,sexo,especie,raza,color,tutor_fk,doctor_fk) "
        "SELECT $1,NULLIF($2,'')::integer,NULLIF($3,''),$4,NULLIF($5,''),"
        "NULLIF($6,''),NULLIF($7,''),$8 "
        "WHERE NULLIF($7,'') IS NULL OR EXISTS (SELECT 1 FROM public.tutor t "
        "WHERE t.rut=$7 AND t.doctor_fk=$8) RETURNING *",
        8, NULL, values, NULL, NULL, 0);
}

PGresult *pacienteUpdate(PGconn *conn, const Sesion *sesion, const char *id,
                         const Paciente *paciente) {
    const char *values[] = {paciente->nombre, paciente->edad, paciente->sexo,
        paciente->especie, paciente->raza, paciente->color, paciente->tutor_fk,
        id, sesion->rut};
    return PQexecParams(conn,
        "UPDATE public.paciente SET nombre=COALESCE(NULLIF($1,''),nombre), "
        "edad=COALESCE(NULLIF($2,'')::integer,edad), "
        "sexo=COALESCE(NULLIF($3,''),sexo), especie=COALESCE(NULLIF($4,''),especie), "
        "raza=COALESCE(NULLIF($5,''),raza), color=COALESCE(NULLIF($6,''),color), "
        "tutor_fk=COALESCE(NULLIF($7,''),tutor_fk) "
        "WHERE id=$8::integer AND doctor_fk=$9 "
        "AND (NULLIF($7,'') IS NULL OR EXISTS (SELECT 1 FROM public.tutor t "
        "WHERE t.rut=$7 AND t.doctor_fk=$9)) RETURNING *",
        9, NULL, values, NULL, NULL, 0);
}

PGresult *pacienteDelete(PGconn *conn, const Sesion *sesion, const char *id) {
    const char *values[] = {id, sesion->rut};
    return PQexecParams(conn,
        "DELETE FROM public.paciente WHERE id=$1::integer AND doctor_fk=$2 RETURNING *",
        2, NULL, values, NULL, NULL, 0);
}
