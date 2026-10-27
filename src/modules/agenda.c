#include <libpq-fe.h>
#include "agenda.h"

PGresult *agendaList(PGconn *conn, const Sesion *sesion) {
    const char *values[] = {sesion->rut};
    return PQexecParams(conn,
        "SELECT a.*, t.domicilio FROM public.agenda a "
        "JOIN public.paciente p ON p.id=a.id_paciente_fk "
        "LEFT JOIN public.tutor t ON t.rut=p.tutor_fk "
        "WHERE p.doctor_fk=$1 ORDER BY a.fecha_hora",
        1, NULL, values, NULL, NULL, 0);
}

PGresult *agendaGet(PGconn *conn, const Sesion *sesion, const char *id) {
    const char *values[] = {id, sesion->rut};
    return PQexecParams(conn,
        "SELECT a.*, t.domicilio FROM public.agenda a "
        "JOIN public.paciente p ON p.id=a.id_paciente_fk "
        "LEFT JOIN public.tutor t ON t.rut=p.tutor_fk "
        "WHERE a.id=$1::integer AND p.doctor_fk=$2",
        2, NULL, values, NULL, NULL, 0);
}

PGresult *agendaListByPaciente(PGconn *conn, const Sesion *sesion,
                               const char *pacienteId) {
    const char *values[] = {pacienteId, sesion->rut};
    return PQexecParams(conn,
        "SELECT a.*, t.domicilio FROM public.agenda a "
        "JOIN public.paciente p ON p.id=a.id_paciente_fk "
        "LEFT JOIN public.tutor t ON t.rut=p.tutor_fk "
        "WHERE p.id=$1::integer AND p.doctor_fk=$2 ORDER BY a.fecha_hora",
        2, NULL, values, NULL, NULL, 0);
}

PGresult *agendaCreate(PGconn *conn, const Sesion *sesion,
                       const Agenda *agenda) {
    const char *values[] = {agenda->fechaHora, agenda->notas,
                            agenda->idPaciente, sesion->rut};
    return PQexecParams(conn,
        "INSERT INTO public.agenda(fecha_hora,notas,id_paciente_fk) "
        "SELECT $1::timestamptz,NULLIF($2,''),p.id FROM public.paciente p "
        "WHERE p.id=$3::integer AND p.doctor_fk=$4 RETURNING *",
        4, NULL, values, NULL, NULL, 0);
}

PGresult *agendaUpdate(PGconn *conn, const Sesion *sesion, const char *id,
                       const Agenda *agenda, bool hasFecha, bool hasNotas) {
    return PQexecParams(conn,
        "UPDATE public.agenda a SET "
        "fecha_hora=CASE WHEN $1::boolean THEN $2::timestamptz ELSE a.fecha_hora END, "
        "notas=CASE WHEN $3::boolean THEN NULLIF($4,'') ELSE a.notas END "
        "FROM public.paciente p WHERE a.id=$5::integer "
        "AND a.id_paciente_fk=p.id AND p.doctor_fk=$6 RETURNING a.*",
        6, NULL,
        (const char *const[]){hasFecha ? "true" : "false", agenda->fechaHora,
            hasNotas ? "true" : "false", agenda->notas, id, sesion->rut},
        NULL, NULL, 0);
}

PGresult *agendaDelete(PGconn *conn, const Sesion *sesion, const char *id) {
    const char *values[] = {id, sesion->rut};
    return PQexecParams(conn,
        "DELETE FROM public.agenda a USING public.paciente p "
        "WHERE a.id=$1::integer AND a.id_paciente_fk=p.id "
        "AND p.doctor_fk=$2 RETURNING a.*",
        2, NULL, values, NULL, NULL, 0);
}
