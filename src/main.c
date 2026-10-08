#include <stdio.h>
#include <stdlib.h>
#include "db.h"
#include "tutor.h"

int main ()
{
   PGconn *conn = db_connect();
    
   //createTutor(conn);  
   readAllTutor(conn);
   //readTutor(conn);
   //PQfinish(conn);
   deleteTutor(conn);
   return 0;


}
