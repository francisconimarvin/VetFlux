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

void createTutor(PGconn *conn, const Sesion *sesion);
void readAllTutor(PGconn *conn, const Sesion *sesion);
void readTutor(PGconn *conn, const Sesion *sesion);
void updateTutor(PGconn *conn, const Sesion *sesion);
void deleteTutor(PGconn *conn, const Sesion *sesion);

#endif
