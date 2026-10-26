#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <libpq-fe.h>
#include "login.h"

typedef struct inventario
{
    char cantidad[6];
} Inventario;

PGresult *inventarioList(PGconn *conn, const Sesion *sesion);
PGresult *inventarioUpdate(PGconn *conn, const Sesion *sesion, const char *instrumentoId, const Inventario *inventario);

#endif
