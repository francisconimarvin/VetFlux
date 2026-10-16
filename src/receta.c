#include <stdio.h>
#include <stdlib.h>

#include "receta.h"
#include "utils.h"

void createReceta(PGconn *conn, const Sesion *sesion)
{
    Receta receta;

    printf("ID del paciente:\n");
    getString(receta.idPaciente, sizeof(receta.idPaciente));

    printf("ID del medicamento:\n");
    getString(receta.idMedicamnento, sizeof(receta.idMedicamnento));

    printf("Dosis del medicamento:\n");
    getString(receta.dosis, sizeof(receta.dosis));

    if (atoi(receta.idPaciente) <= 0 ||
        atoi(receta.idMedicamnento) <= 0 ||
        receta.dosis[0] == '\0')
    {
        printf("Datos invalidos.\n");
        return;
    }

    const char *query =
        "INSERT INTO public.receta "
        "(dosis, paciente_fk, medicamento_fk) "
        "SELECT $1::varchar, p.id, m.id "
        "FROM public.paciente p "
        "CROSS JOIN public.medicamentos m "
        "WHERE p.id = $2::integer "
        "AND m.id = $3::integer "
        "AND p.doctor_fk = $4 "
        "RETURNING *";

    const char *paramValues[4] = {
        receta.dosis,
        receta.idPaciente,
        receta.idMedicamnento,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn, query, 4, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al crear la receta: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    if (PQntuples(res) == 0)
    {
        printf("No se encontro el paciente o el medicamento, "
               "o el paciente no te pertenece.\n");
    }
    else
    {
        printf("Receta creada correctamente.\n");
        printQueryResult(res);
    }

    PQclear(res);
}

void readAllReceta(PGconn *conn, const Sesion *sesion)
{
    const char *query =
        "SELECT "
        "r.id AS id_receta, "
        "r.dosis, "
        "p.id AS id_paciente, "
        "p.nombre AS nombre_paciente, "
        "m.id AS id_medicamento, "
        "m.nombre AS nombre_medicamento "
        "FROM public.receta r "
        "JOIN public.paciente p "
        "ON p.id = r.paciente_fk "
        "JOIN public.medicamentos m "
        "ON m.id = r.medicamento_fk "
        "WHERE p.doctor_fk = $1 "
        "ORDER BY r.id";

    const char *paramValues[1] = {
        sesion->rut};

    PGresult *res = PQexecParams(
        conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al consultar las recetas: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printQueryResult(res);
    PQclear(res);
}

void readReceta(PGconn *conn, const Sesion *sesion)
{
    char idPaciente[12];

    printf("ID del paciente:\n");
    getString(idPaciente, sizeof(idPaciente));

    if (atoi(idPaciente) <= 0)
    {
        printf("ID del paciente invalido.\n");
        return;
    }

    const char *query =
        "SELECT "
        "r.id AS id_receta, "
        "r.dosis, "
        "p.id AS id_paciente, "
        "p.nombre AS nombre_paciente, "
        "m.id AS id_medicamento, "
        "m.nombre AS nombre_medicamento "
        "FROM public.receta r "
        "JOIN public.paciente p "
        "ON p.id = r.paciente_fk "
        "JOIN public.medicamentos m "
        "ON m.id = r.medicamento_fk "
        "WHERE p.id = $1::integer "
        "AND p.doctor_fk = $2 "
        "ORDER BY r.id";

    const char *paramValues[2] = {
        idPaciente,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al consultar las recetas: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printQueryResult(res);
    PQclear(res);
}

void updateReceta(PGconn *conn, const Sesion *sesion)
{
    char idReceta[12];
    char dosis[50];

    printf("ID de la receta a actualizar:\n");
    getString(idReceta, sizeof(idReceta));

    printf("Nueva dosis:\n");
    getString(dosis, sizeof(dosis));

    if (atoi(idReceta) <= 0 || dosis[0] == '\0')
    {
        printf("Datos invalidos.\n");
        return;
    }

    const char *query =
        "UPDATE public.receta r "
        "SET dosis = $1::varchar "
        "FROM public.paciente p "
        "WHERE r.id = $2::integer "
        "AND p.id = r.paciente_fk "
        "AND p.doctor_fk = $3 "
        "RETURNING r.*";

    const char *paramValues[3] = {
        dosis,
        idReceta,
        sesion->rut};

    PGresult *res = PQexecParams(
        conn, query, 3, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al actualizar la receta: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    if (PQntuples(res) == 0)
    {
        printf("No se encontro la receta o no te pertenece.\n");
    }
    else
    {
        printf("Receta actualizada correctamente.\n");
        printQueryResult(res);
    }

    PQclear(res);
}