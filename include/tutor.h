#ifndef TUTOR_H
#define TUTOR_H

#include <stdio.h>
#include <libpq-fe.h>

typedef struct tutor {
    char rut[16];
    char nombre[100];
    char email[150];
    char telefono[11];
    char domicilio[200];
} Tutor;

void createTutor(PGconn *conn);
void readTutor(PGconn *conn);
void updateTutor(PGconn *conn);
void deleteTutor(PGconn *conn);

#endif
