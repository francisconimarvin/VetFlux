#include "libpq-fe.h"
#include "db.h"


PGconn *dbConnect(void)
{
    PGconn *conn = PQconnectdb("");

    if (conn == NULL || PQstatus(conn) != CONNECTION_OK)
    {
        if (conn != NULL)
            PQfinish(conn);
        return NULL;
    }

    PGresult *res = PQexec(conn,
                 "SELECT pg_catalog.set_config('search_path', '', false)");
    if (res == NULL || PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        if (res != NULL)
            PQclear(res);
        PQfinish(conn);
        return NULL;
    }

    PQclear(res);
    return conn;
}
