#include <stdio.h>
#include "libpq-fe.h"
#include "db.h"
#include <stdlib.h>


PGconn *dbConnect(void)
{
    const char *conninfo = getenv("DATABASE_URL");
    if (conninfo == NULL || conninfo[0] == '\0') {
        fprintf(stderr, "Configura DATABASE_URL para conectar a PostgreSQL\n");
        return NULL;
    }

    PGconn *conn;
    PGresult *res;

    conn = PQconnectdb(conninfo);

    if (conn == NULL || PQstatus(conn) != CONNECTION_OK)
    {
        if (conn != NULL)
            fprintf(stderr, "%s", PQerrorMessage(conn));
        return conn;
    } else 
    {
        printf("Connected to VetFlux\n");
    }

    res = PQexec(conn,
                 "SELECT pg_catalog.set_config('search_path', '', false)");
    if (res == NULL || PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
        if (res != NULL)
            PQclear(res);
        PQfinish(conn);
        return NULL;
    }

    PQclear(res);
    return conn;
}
