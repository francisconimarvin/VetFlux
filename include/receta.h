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


PGresult *recetaList(PGconn *conn, const Sesion *sesion);
PGresult *recetaGet(PGconn *conn, const Sesion *sesion, const char *id);
PGresult *recetaListByPaciente(PGconn *conn, const Sesion *sesion, const char *pacienteId);
PGresult *recetaCreate(PGconn *conn, const Sesion *sesion, const Receta *receta);
PGresult *recetaUpdate(PGconn *conn, const Sesion *sesion, const char *id, const Receta *receta);

#endif
