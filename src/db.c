#include <stdio.h>
#include "libpq-fe.h"
#include "db.h"
#include <stdlib.h>


PGconn *db_connect(void)
{
    const char *conninfo;
    conninfo = "host=localhost port=5432 dbname=VetFlux user=marvin";

    PGconn *conn;
    PGresult *res;

    conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "%s", PQerrorMessage(conn));
    } else 
    {
        printf("Connected to VetFlux\n");
    }

    res = PQexec(conn,
                 "SELECT pg_catalog.set_config('search_path', '', false)");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
        PQclear(res);
    }

    PQclear(res);
    return conn;
}
