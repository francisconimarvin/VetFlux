#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "agenda.h"
#include "login.h"
#include <libpq-fe.h>
#include "utils.h"

void createAgenda(PGconn *conn, const Sesion *sesion)
{
    Agenda agenda;

    printf("Selecciona una hora para agendar:\n");
    fgets(agenda.fechaHora, sizeof(agenda.fechaHora), stdin);
    agenda.fechaHora[strcspn(agenda.fechaHora, "\n")] = '\0';

    printf("Notas sobre el agendamiento (opcional):\n");
    fgets(agenda.notas, sizeof(agenda.notas), stdin);
    agenda.notas[strcspn(agenda.notas, "\n")] = '\0';

    printf("Ingrese el ID del paciente:\n");
    fgets(agenda.idPaciente, sizeof(agenda.idPaciente), stdin);
    agenda.idPaciente[strcspn(agenda.idPaciente, "\n")] = '\0';

    const char *paramValues[4] = {
        agenda.fechaHora,
        agenda.notas,
        agenda.idPaciente,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn,
        "INSERT INTO public.agenda "
        "(fecha_hora, notas, id_paciente_fk) "
        "SELECT $1::timestamptz, NULLIF($2, ''), p.id "
        "FROM public.paciente p "
        "WHERE p.id = $3::integer "
        "AND p.doctor_fk = $4",
        4,
        NULL,
        paramValues,
        NULL,
        NULL,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "INSERT failed: %s\n", PQerrorMessage(conn));
    }
    else if (atoi(PQcmdTuples(res)) == 0)
    {
        printf("No existe un paciente con ese ID o no tienes acceso a él.\n");
    }
    else
    {
        printf("Agenda creada exitosamente.\n");
    }

    PQclear(res);
}

void readAllAgenda(PGconn *conn, const Sesion *sesion)
{
    const char *paramValues[1] = {
        sesion->rut};

    PGresult *res = PQexecParams(
        conn,
        "SELECT a.*, t.domicilio "
        "FROM public.agenda a "
        "JOIN public.paciente p ON p.id = a.id_paciente_fk "
        "LEFT JOIN public.tutor t ON t.rut = p.tutor_fk "
        "WHERE p.doctor_fk = $1 "
        "ORDER BY a.fecha_hora",
        1,
        NULL,
        paramValues,
        NULL,
        NULL,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printQueryResult(res);
    PQclear(res);
}

void readAgenda(PGconn *conn, const Sesion *sesion)
{
    char idPaciente[12];

    printf("Ingrese el ID del paciente:\n");
    fgets(idPaciente, sizeof(idPaciente), stdin);
    idPaciente[strcspn(idPaciente, "\n")] = '\0';

    const char *paramValues[2] = {
        idPaciente,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn,
        "SELECT a.*, t.domicilio "
        "FROM public.agenda a "
        "JOIN public.paciente p ON p.id = a.id_paciente_fk "
        "LEFT JOIN public.tutor t ON t.rut = p.tutor_fk "
        "WHERE p.id = $1::integer "
        "AND p.doctor_fk = $2 "
        "ORDER BY a.fecha_hora",
        2,
        NULL,
        paramValues,
        NULL,
        NULL,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printQueryResult(res);
    PQclear(res);
}

void updateAgenda(PGconn *conn, const Sesion *sesion)
{
    char id[12];
    char value[200];

    printf("ID de la agenda que quieres actualizar:\n");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';

    printf("¿Qué quieres actualizar?\n");
    printf("1. Fecha y hora\n");
    printf("2. Notas\n");

    int option = getInteger();

    if (option == -1)
    {
        printf("Opción inválida.\n");
        return;
    }

    const char *query;
    const char *field;

    switch (option)
    {
    case 1:
        field = "fecha y hora";
        query =
            "UPDATE public.agenda a "
            "SET fecha_hora = $1::timestamptz "
            "FROM public.paciente p "
            "WHERE a.id = $2::integer "
            "AND a.id_paciente_fk = p.id "
            "AND p.doctor_fk = $3";
        printf("Nueva fecha y hora (YYYY-MM-DD HH:MM):\n");
        break;

    case 2:
        field = "notas";
        query =
            "UPDATE public.agenda a "
            "SET notas = NULLIF($1, '') "
            "FROM public.paciente p "
            "WHERE a.id = $2::integer "
            "AND a.id_paciente_fk = p.id "
            "AND p.doctor_fk = $3";
        printf("Nuevas notas:\n");
        break;

    default:
        printf("Opción inválida.\n");
        return;
    }

    fgets(value, sizeof(value), stdin);
    value[strcspn(value, "\n")] = '\0';

    const char *paramValues[3] = {
        value,
        id,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn,
        query,
        3,
        NULL,
        paramValues,
        NULL,
        NULL,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "UPDATE failed: %s\n", PQerrorMessage(conn));
    }
    else if (atoi(PQcmdTuples(res)) == 0)
    {
        printf("No existe una agenda con ese ID o no tienes acceso a ella.\n");
    }
    else
    {
        printf("Agenda: %s actualizada exitosamente.\n", field);
    }

    PQclear(res);
}

void deleteAgenda(PGconn *conn, const Sesion *sesion)
{
    char id[12];

    printf("ID de la agenda que quieres eliminar:\n");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';

    const char *paramValues[2] = {id, sesion->rut};

    PGresult *res = PQexecParams(
        conn,
        "DELETE FROM public.agenda a "
        "USING public.paciente p "
        "WHERE a.id = $1::integer "
        "AND a.id_paciente_fk = p.id "
        "AND p.doctor_fk = $2 "
        "RETURNING a.*",
        2,
        NULL,
        paramValues,
        NULL,
        NULL,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "DELETE failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);

    if (rows == 0)
    {
        printf("No existe una agenda con ese ID o no tienes acceso a ella.\n");
    }
    else
    {
        printf("Agenda eliminada exitosamente:\n");

        for (int j = 0; j < PQnfields(res); j++)
        {
            printf("%-20s", PQfname(res, j));
        }
        printf("\n");

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < PQnfields(res); j++)
            {
                printf("%-20s", PQgetvalue(res, i, j));
            }
            printf("\n");
        }
    }

    PQclear(res);
}