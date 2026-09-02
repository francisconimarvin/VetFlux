#include <stdio.h>
#include "libpq-fe.h"
#include "tutor.h"

PGconn *db_connect(const char *conninfo);

int main(void) {
    printf("Hello world!\n");
    
    /* You should create a .pgpass
     * Read the PG Documentation regarding passfiles.
     * */
    const char *conninfo = "dbname=VetFlux user=marvin";
    PGconn *conn = db_connect(conninfo);

    if (conn==NULL) {
        return 1;
    }

    printf("Connected to database: %s\n", PQdb(conn));
    create_Tutor(conn); 

    return 0;
}

