#ifndef TUTOR_H
#define TUTOR_H

#include <stdio.h>
#include <libpq-fe.h>
#include "login.h"

typedef struct tutor {
    char rut[16];
    char nombre[100];
    char email[150];
    char telefono[11];
    char domicilio[200];
} Tutor;

PGresult *tutorList(PGconn *conn, const Sesion *sesion);
PGresult *tutorGet(PGconn *conn, const Sesion *sesion, const char *rut);
PGresult *tutorCreate(PGconn *conn, const Sesion *sesion, const Tutor *tutor);
PGresult *tutorUpdate(PGconn *conn, const Sesion *sesion, const char *rut, const Tutor *tutor);
PGresult *tutorDelete(PGconn *conn, const Sesion *sesion, const char *rut);

#endif
