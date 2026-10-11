#ifndef MENU_H
#define MENU_H

#include <libpq-fe.h>
#include "login.h"

void mostrarMenu(PGconn *conn, const Sesion *sesion);

#endif
