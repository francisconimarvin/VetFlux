#ifndef TUTOR_H
#define TUTOR_H

#include "libpq-fe.h"

void menu_Tutor(PGconn *conn);

typedef struct {
    char rut[14];
    char nombre[101];
    char telefono[11];
    char email[101];
    char domicilio[101];
} Tutor;

// CRUD
void create_Tutor(PGconn *conn);
void read_Tutor(PGconn *conn);
void update_Tutor(PGconn *conn);
void delete_Tutor(PGconn *conn);

#endif
