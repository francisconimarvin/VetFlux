#include <stdio.h>
#include <stdlib.h>

#include "fichaMedica.h"
#include "utils.h"

void createFichaMedica(PGconn *conn, const Sesion *sesion)
{
    FichaMedica ficha;

    printf("ID del paciente:\n");
    getString(ficha.idPaciente, sizeof(ficha.idPaciente));

    printf("Titulo de la ficha medica:\n");
    getString(ficha.titulo, sizeof(ficha.titulo));

    printf("Descripcion de la ficha medica:\n");
    getString(ficha.descripcion, sizeof(ficha.descripcion));

    if (atoi(ficha.idPaciente) <= 0)
    {
        printf("ID del paciente invalido.\n");
        return;
    }

    const char *query =
        "INSERT INTO public.ficha_medica "
        "(titulo, descripcion, id_paciente_fk) "
        "SELECT $1, $2, p.id "
        "FROM public.paciente p "
        "WHERE p.id = $3::integer "
        "AND p.doctor_fk = $4 "
        "RETURNING *";

    const char *paramValues[4] = {
        ficha.titulo,
        ficha.descripcion,
        ficha.idPaciente,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn, query, 4, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al crear la ficha medica: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    if (PQntuples(res) == 0)
    {
        printf("No se encontro el paciente o no te pertenece.\n");
    }
    else
    {
        printf("Ficha medica creada correctamente.\n");
        printQueryResult(res);
    }

    PQclear(res);
}

void readAllFichaMedica(PGconn *conn, const Sesion *sesion)
{
    const char *query =
        "SELECT "
        "f.id AS id_ficha, "
        "f.fecha_ficha, "
        "p.id AS id_paciente, "
        "p.nombre AS nombre_paciente, "
        "t.rut AS rut_tutor "
        "FROM public.ficha_medica f "
        "JOIN public.paciente p "
        "ON p.id = f.id_paciente_fk "
        "LEFT JOIN public.tutor t "
        "ON t.rut = p.tutor_fk "
        "WHERE p.doctor_fk = $1 "
        "ORDER BY f.id";

    const char *paramValues[1] = {sesion->rut};

    PGresult *res = PQexecParams(
        conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al consultar las fichas medicas: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printQueryResult(res);
    PQclear(res);
}

void readFichaMedica(PGconn *conn, const Sesion *sesion)
{
    char fichaId[12];

    printf("ID de la ficha medica:\n");
    getString(fichaId, sizeof(fichaId));

    if (atoi(fichaId) <= 0)
    {
        printf("ID de ficha invalido.\n");
        return;
    }

    const char *query =
        "SELECT "
        "f.id AS id_ficha, "
        "f.fecha_ficha, "
        "f.titulo, "
        "f.descripcion, "
        "p.id AS id_paciente, "
        "p.nombre AS nombre_paciente, "
        "t.rut AS rut_tutor "
        "FROM public.ficha_medica f "
        "JOIN public.paciente p "
        "ON p.id = f.id_paciente_fk "
        "LEFT JOIN public.tutor t "
        "ON t.rut = p.tutor_fk "
        "WHERE f.id = $1::integer "
        "AND p.doctor_fk = $2";

    const char *paramValues[2] = {
        fichaId,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al consultar la ficha medica: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printQueryResult(res);
    PQclear(res);
}

void updateFichaMedica(PGconn *conn, const Sesion *sesion)
{
    char fichaId[12];
    FichaMedica ficha;

    printf("ID de la ficha medica a actualizar:\n");
    getString(fichaId, sizeof(fichaId));

    if (atoi(fichaId) <= 0)
    {
        printf("ID de ficha invalido.\n");
        return;
    }

    printf("Nuevo titulo:\n");
    getString(ficha.titulo, sizeof(ficha.titulo));

    printf("Nueva descripcion:\n");
    getString(ficha.descripcion, sizeof(ficha.descripcion));

    const char *query =
        "UPDATE public.ficha_medica f "
        "SET titulo = $1, descripcion = $2 "
        "FROM public.paciente p "
        "WHERE f.id = $3::integer "
        "AND p.id = f.id_paciente_fk "
        "AND p.doctor_fk = $4 "
        "RETURNING f.*";

    const char *paramValues[4] = {
        ficha.titulo,
        ficha.descripcion,
        fichaId,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn, query, 4, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al actualizar la ficha medica: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    if (PQntuples(res) == 0)
    {
        printf("No se encontro la ficha o no te pertenece.\n");
    }
    else
    {
        printf("Ficha medica actualizada correctamente.\n");
        printQueryResult(res);
    }

    PQclear(res);
}

void deleteFichaMedica(PGconn *conn, const Sesion *sesion)
{
    char fichaId[12];

    printf("ID de la ficha medica a eliminar:\n");
    getString(fichaId, sizeof(fichaId));

    if (atoi(fichaId) <= 0)
    {
        printf("ID de ficha invalido.\n");
        return;
    }

    const char *query =
        "DELETE FROM public.ficha_medica f "
        "USING public.paciente p "
        "WHERE f.id = $1::integer "
        "AND p.id = f.id_paciente_fk "
        "AND p.doctor_fk = $2 "
        "RETURNING f.*";

    const char *paramValues[2] = {
        fichaId,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al eliminar la ficha medica: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    if (PQntuples(res) == 0)
    {
        printf("No se encontro la ficha o no te pertenece.\n");
    }
    else
    {
        printf("Ficha medica eliminada correctamente.\n");
        printQueryResult(res);
    }

    PQclear(res);
}