#include <stdio.h>
#include "db.h"

int main ()
{
    PGconn *conn = db_connect();
    return 0;
}
