#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include "utils.h"
#include "inventario.h"

void readAllInventario(PGconn *conn, const Sesion *sesion)
{
    const char *query =
        "SELECT i.id AS instrumento_id, i.nombre, inv.cantidad "
        "FROM public.inventario inv "
        "JOIN public.instrumentos i ON i.id = inv.instrumento_fk "
        "WHERE inv.doctor_fk = $1 "
        "ORDER BY i.nombre";

    const char *paramValues[1] = {sesion->rut};

    PGresult *res = PQexecParams(
        conn,
        query,
        1,
        NULL,
        paramValues,
        NULL,
        NULL,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("Error al consultar el inventario: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printQueryResult(res);
    PQclear(res);
}

void updateInventario(PGconn *conn, const Sesion *sesion)
{
    char instrumentoId[12];
    char cantidad[12];

    printf("ID del instrumento a actualizar:\n");
    getString(instrumentoId, sizeof(instrumentoId));

    printf("Nueva cantidad:\n");
    getString(cantidad, sizeof(cantidad));

    if (atoi(instrumentoId) <= 0 || atoi(cantidad) < 0)
    {
        printf("ID o cantidad inválidos.\n");
        return;
    }

    const char *query =
        "UPDATE public.inventario "
        "SET cantidad = $1::integer "
        "WHERE instrumento_fk = $2::integer "
        "AND doctor_fk = $3";

    const char *paramValues[3] = {
        cantidad,
        instrumentoId,
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
        printf("Error al actualizar el inventario: %s\n",
               PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    if (atoi(PQcmdTuples(res)) == 0)
    {
        printf("No se encontró ese instrumento en tu inventario.\n");
    }
    else
    {
        printf("Inventario actualizado correctamente.\n");
    }

    PQclear(res);
}