#ifndef AGENDA_H
#define AGENDA_H

#include <stdio.h>
#include <stdbool.h>
#include "login.h"
#include <libpq-fe.h>

typedef struct agenda
{
    char fechaHora[22];
    char notas[1024];
    char idPaciente[4];
} Agenda;

PGresult *agendaList(PGconn *conn, const Sesion *sesion);
PGresult *agendaGet(PGconn *conn, const Sesion *sesion, const char *id);
PGresult *agendaListByPaciente(PGconn *conn, const Sesion *sesion, const char *pacienteId);
PGresult *agendaCreate(PGconn *conn, const Sesion *sesion, const Agenda *agenda);
PGresult *agendaUpdate(PGconn *conn, const Sesion *sesion, const char *id, const Agenda *agenda, bool hasFecha, bool hasNotas);
PGresult *agendaDelete(PGconn *conn, const Sesion *sesion, const char *id);

#endif
