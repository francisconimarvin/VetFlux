#ifndef AGENDA_H
#define AGENDA_H

#include <stdio.h>
#include "login.h"
#include <libpq-fe.h>

typedef struct agenda
{
    char fechaHora[22];
    char notas[1024];
    char idPaciente[4];
} Agenda;

void createAgenda(PGconn *conn, const Sesion *sesion);
void readAllAgenda(PGconn *conn, const Sesion *sesion);
void readAgenda(PGconn *conn, const Sesion *sesion);
void updateAgenda(PGconn *conn, const Sesion *sesion);
void deleteAgenda(PGconn *conn, const Sesion *sesion);

#endif
