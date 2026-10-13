#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <libpq-fe.h>
#include "login.h"

typedef struct inventario
{
    char cantidad[6];
} Inventario;

void readAllInventario(PGconn *conn, const Sesion *sesion);
void updateInventario(PGconn *conn, const Sesion *sesion);

#endif