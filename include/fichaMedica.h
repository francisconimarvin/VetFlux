#ifndef FICHA_MEDICA_H
#define FICHA_MEDICA_H

#include <stdbool.h>
#include <libpq-fe.h>
#include "login.h"

typedef struct fichaMedica {
    char fecha_ficha[30];
    char titulo[101];
    char descripcion[1024];
    char idPaciente[12];
} FichaMedica;

PGresult *fichaMedicaList(PGconn *conn, const Sesion *sesion);
PGresult *fichaMedicaGet(PGconn *conn, const Sesion *sesion, const char *id);
PGresult *fichaMedicaCreate(PGconn *conn, const Sesion *sesion, const FichaMedica *ficha);
PGresult *fichaMedicaUpdate(PGconn *conn, const Sesion *sesion, const char *id, const FichaMedica *ficha, bool hasTitulo, bool hasDescripcion);
PGresult *fichaMedicaDelete(PGconn *conn, const Sesion *sesion, const char *id);

#endif
