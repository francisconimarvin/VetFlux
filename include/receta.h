#ifndef RECETA_H
#define RECETA_H

#include <libpq-fe.h>
#include "login.h"

typedef struct receta
{
    char dosis[50];
    char idPaciente[12];
    char idMedicamnento[12];
} Receta;


void createReceta(PGconn *conn, const Sesion *sesion);
void readAllReceta(PGconn *conn, const Sesion *sesion);
void readReceta(PGconn *conn, const Sesion *sesion);
void updateReceta(PGconn *conn, const Sesion *sesion);

#endif