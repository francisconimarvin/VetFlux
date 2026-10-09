#include <stdio.h>
#include <stdlib.h>
#include "db.h"
#include "tutor.h"
#include "menu.h"
int main ()
{
   PGconn *conn = dbConnect();
    
   mostrarMenu(conn);
   PQfinish(conn);
   return 0;


}
