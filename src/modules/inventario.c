#include <libpq-fe.h>
#include "inventario.h"

PGresult *inventarioList(PGconn *conn, const Sesion *sesion) {
    const char *values[] = {sesion->rut};
    return PQexecParams(conn,
        "SELECT i.id AS instrumento_id,i.nombre,inv.cantidad "
        "FROM public.inventario inv JOIN public.instrumentos i ON i.id=inv.instrumento_fk "
        "WHERE inv.doctor_fk=$1 ORDER BY i.nombre",
        1, NULL, values, NULL, NULL, 0);
}

PGresult *inventarioUpdate(PGconn *conn, const Sesion *sesion,
                           const char *instrumentoId,
                           const Inventario *inventario) {
    const char *values[] = {inventario->cantidad, instrumentoId, sesion->rut};
    return PQexecParams(conn,
        "UPDATE public.inventario SET cantidad=$1::integer "
        "WHERE instrumento_fk=$2::integer AND doctor_fk=$3 RETURNING *",
        3, NULL, values, NULL, NULL, 0);
}
