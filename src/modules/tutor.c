#include <libpq-fe.h>
#include "tutor.h"

PGresult *tutorList(PGconn *conn, const Sesion *sesion) {
    const char *values[] = {sesion->rut};
    return PQexecParams(conn,
        "SELECT * FROM public.tutor WHERE doctor_fk=$1 ORDER BY rut",
        1, NULL, values, NULL, NULL, 0);
}

PGresult *tutorGet(PGconn *conn, const Sesion *sesion, const char *rut) {
    const char *values[] = {rut, sesion->rut};
    return PQexecParams(conn,
        "SELECT * FROM public.tutor WHERE rut=$1 AND doctor_fk=$2",
        2, NULL, values, NULL, NULL, 0);
}

PGresult *tutorCreate(PGconn *conn, const Sesion *sesion, const Tutor *tutor) {
    const char *values[] = {tutor->rut, tutor->nombre, tutor->email,
                            tutor->telefono, tutor->domicilio, sesion->rut};
    return PQexecParams(conn,
        "INSERT INTO public.tutor(rut,nombre,email,telefono,domicilio,doctor_fk) "
        "VALUES($1,$2,$3,$4,$5,$6) RETURNING *",
        6, NULL, values, NULL, NULL, 0);
}

PGresult *tutorUpdate(PGconn *conn, const Sesion *sesion, const char *rut,
                      const Tutor *tutor) {
    const char *values[] = {tutor->nombre, tutor->email, tutor->telefono,
                            tutor->domicilio, rut, sesion->rut};
    return PQexecParams(conn,
        "UPDATE public.tutor SET nombre=COALESCE(NULLIF($1,''),nombre), "
        "email=COALESCE(NULLIF($2,''),email), "
        "telefono=COALESCE(NULLIF($3,''),telefono), "
        "domicilio=COALESCE(NULLIF($4,''),domicilio) "
        "WHERE rut=$5 AND doctor_fk=$6 RETURNING *",
        6, NULL, values, NULL, NULL, 0);
}

PGresult *tutorDelete(PGconn *conn, const Sesion *sesion, const char *rut) {
    const char *values[] = {rut, sesion->rut};
    return PQexecParams(conn,
        "DELETE FROM public.tutor WHERE rut=$1 AND doctor_fk=$2 RETURNING *",
        2, NULL, values, NULL, NULL, 0);
}
