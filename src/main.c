#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "db.h"
#include "menu.h"
#include "login.h"

int main(void)
{
    PGconn *conn = dbConnect();

    int log = login(conn);
    //  mostrarMenu(conn);    
    PQfinish(conn);

    return 0;
}
