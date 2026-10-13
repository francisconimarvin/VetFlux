#ifndef FICHA_MEDICA_H
#define FICHA_MEDICA_H

#include <libpq-fe.h>
#include "login.h"

typedef struct fichaMedica {
    char fecha_ficha[30];
    char titulo[101];
    char descripcion[1024];
    char idPaciente[12];
} FichaMedica;

void createFichaMedica(PGconn *conn, const Sesion *sesion);
void readAllFichaMedica(PGconn *conn, const Sesion *sesion);
void readFichaMedica(PGconn *conn, const Sesion *sesion);
void updateFichaMedica(PGconn *conn, const Sesion *sesion);
void deleteFichaMedica(PGconn *conn, const Sesion *sesion);

#endif