#include <stdio.h>
#include "db.h"
#include "tutor.h"

int main ()
{
    PGconn *conn = db_connect();
    
    createTutor(conn);  
    PQfinish(conn);
    return 0;

}
