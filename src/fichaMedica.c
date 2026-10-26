#include <libpq-fe.h>
#include "fichaMedica.h"

PGresult *fichaMedicaList(PGconn *conn, const Sesion *sesion) {
    const char *values[] = {sesion->rut};
    return PQexecParams(conn,
        "SELECT f.id AS id_ficha,f.fecha_ficha,p.id AS id_paciente, "
        "p.nombre AS nombre_paciente,t.rut AS rut_tutor "
        "FROM public.ficha_medica f JOIN public.paciente p ON p.id=f.id_paciente_fk "
        "LEFT JOIN public.tutor t ON t.rut=p.tutor_fk "
        "WHERE p.doctor_fk=$1 ORDER BY f.id",
        1, NULL, values, NULL, NULL, 0);
}

PGresult *fichaMedicaGet(PGconn *conn, const Sesion *sesion, const char *id) {
    const char *values[] = {id, sesion->rut};
    return PQexecParams(conn,
        "SELECT f.id AS id_ficha,f.fecha_ficha,f.titulo,f.descripcion, "
        "p.id AS id_paciente,p.nombre AS nombre_paciente,t.rut AS rut_tutor "
        "FROM public.ficha_medica f JOIN public.paciente p ON p.id=f.id_paciente_fk "
        "LEFT JOIN public.tutor t ON t.rut=p.tutor_fk "
        "WHERE f.id=$1::integer AND p.doctor_fk=$2",
        2, NULL, values, NULL, NULL, 0);
}

PGresult *fichaMedicaCreate(PGconn *conn, const Sesion *sesion,
                            const FichaMedica *ficha) {
    const char *values[] = {ficha->titulo, ficha->descripcion,
                            ficha->idPaciente, sesion->rut};
    return PQexecParams(conn,
        "INSERT INTO public.ficha_medica(titulo,descripcion,id_paciente_fk) "
        "SELECT $1,$2,p.id FROM public.paciente p WHERE p.id=$3::integer "
        "AND p.doctor_fk=$4 RETURNING *",
        4, NULL, values, NULL, NULL, 0);
}

PGresult *fichaMedicaUpdate(PGconn *conn, const Sesion *sesion, const char *id,
                            const FichaMedica *ficha, bool hasTitulo,
                            bool hasDescripcion) {
    const char *values[] = {hasTitulo ? "true" : "false", ficha->titulo,
        hasDescripcion ? "true" : "false", ficha->descripcion, id, sesion->rut};
    return PQexecParams(conn,
        "UPDATE public.ficha_medica f SET "
        "titulo=CASE WHEN $1::boolean THEN $2 ELSE f.titulo END, "
        "descripcion=CASE WHEN $3::boolean THEN $4 ELSE f.descripcion END "
        "FROM public.paciente p WHERE f.id=$5::integer "
        "AND p.id=f.id_paciente_fk AND p.doctor_fk=$6 RETURNING f.*",
        6, NULL, values, NULL, NULL, 0);
}

PGresult *fichaMedicaDelete(PGconn *conn, const Sesion *sesion, const char *id) {
    const char *values[] = {id, sesion->rut};
    return PQexecParams(conn,
        "DELETE FROM public.ficha_medica f USING public.paciente p "
        "WHERE f.id=$1::integer AND p.id=f.id_paciente_fk "
        "AND p.doctor_fk=$2 RETURNING f.*",
        2, NULL, values, NULL, NULL, 0);
}
