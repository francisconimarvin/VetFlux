#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "db.h"
#include "menu.h"
#include "login.h"

int main(void)
{
    PGconn *conn = dbConnect();

    Sesion *sesion = login(conn);

    if (sesion != NULL)
    {
        mostrarMenu(conn, sesion);
        free(sesion);
    } else {
        printf("Acceso denegado\n");
    }

    PQfinish(conn);

    return 0;
}
