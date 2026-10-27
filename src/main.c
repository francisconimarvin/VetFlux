#include <stdio.h>
#include <db.h>
#include <stdlib.h>

int main(void)
{
    printf("Hello world! \n");
    PGconn *conn = dbConnect();
    if (conn == NULL) {
        printf("Error de conexión\n");
        return EXIT_FAILURE;
    }
    printf("Conexión exitosa\n");
    PQfinish(conn);
    return EXIT_SUCCESS;
}
