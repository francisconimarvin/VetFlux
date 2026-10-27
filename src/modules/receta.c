#include <libpq-fe.h>
#include "receta.h"

PGresult *recetaList(PGconn *conn, const Sesion *sesion) {
    const char *values[] = {sesion->rut};
    return PQexecParams(conn,
        "SELECT r.id AS id_receta,r.dosis,p.id AS id_paciente, "
        "p.nombre AS nombre_paciente,m.id AS id_medicamento,m.nombre AS nombre_medicamento "
        "FROM public.receta r JOIN public.paciente p ON p.id=r.paciente_fk "
        "JOIN public.medicamentos m ON m.id=r.medicamento_fk "
        "WHERE p.doctor_fk=$1 ORDER BY r.id",
        1, NULL, values, NULL, NULL, 0);
}

PGresult *recetaGet(PGconn *conn, const Sesion *sesion, const char *id) {
    const char *values[] = {id, sesion->rut};
    return PQexecParams(conn,
        "SELECT r.* FROM public.receta r JOIN public.paciente p ON p.id=r.paciente_fk "
        "WHERE r.id=$1::integer AND p.doctor_fk=$2",
        2, NULL, values, NULL, NULL, 0);
}

PGresult *recetaListByPaciente(PGconn *conn, const Sesion *sesion,
                               const char *pacienteId) {
    const char *values[] = {pacienteId, sesion->rut};
    return PQexecParams(conn,
        "SELECT r.id AS id_receta,r.dosis,p.id AS id_paciente, "
        "p.nombre AS nombre_paciente,m.id AS id_medicamento,m.nombre AS nombre_medicamento "
        "FROM public.receta r JOIN public.paciente p ON p.id=r.paciente_fk "
        "JOIN public.medicamentos m ON m.id=r.medicamento_fk "
        "WHERE p.id=$1::integer AND p.doctor_fk=$2 ORDER BY r.id",
        2, NULL, values, NULL, NULL, 0);
}

PGresult *recetaCreate(PGconn *conn, const Sesion *sesion,
                       const Receta *receta) {
    const char *values[] = {receta->dosis, receta->idPaciente,
                            receta->idMedicamnento, sesion->rut};
    return PQexecParams(conn,
        "INSERT INTO public.receta(dosis,paciente_fk,medicamento_fk) "
        "SELECT $1,p.id,m.id FROM public.paciente p CROSS JOIN public.medicamentos m "
        "WHERE p.id=$2::integer AND m.id=$3::integer AND p.doctor_fk=$4 RETURNING *",
        4, NULL, values, NULL, NULL, 0);
}

PGresult *recetaUpdate(PGconn *conn, const Sesion *sesion, const char *id,
                       const Receta *receta) {
    const char *values[] = {receta->dosis, id, sesion->rut};
    return PQexecParams(conn,
        "UPDATE public.receta r SET dosis=$1 FROM public.paciente p "
        "WHERE r.id=$2::integer AND p.id=r.paciente_fk AND p.doctor_fk=$3 RETURNING r.*",
        3, NULL, values, NULL, NULL, 0);
}
