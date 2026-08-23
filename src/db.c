#include <stdio.h>
#include <stdlib.h>
#include "libpq-fe.h"

static void exit_nicely(PGconn *conn) {
    PQfinish(conn);
    exit(1);
}

PGconn *db_connect(const char *conninfo) {
    PGconn *conn;
    
    conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "%s", PQerrorMessage(conn));
        exit_nicely(conn);
    }
    return conn;
}
