#ifndef PACIENTE_H
#define PACIENTE_H

#include <stdio.h>
#include <libpq-fe.h>
#include "login.h"

typedef struct paciente
{
    int id;
    char nombre[100];
    char edad[5];
    char sexo[4];
    char especie[50];
    char raza[100];
    char color[50];
    char tutor_fk[16];
    char doctor_fk[16];
} Paciente;

PGresult *pacienteList(PGconn *conn, const Sesion *sesion);
PGresult *pacienteGet(PGconn *conn, const Sesion *sesion, const char *id);
PGresult *pacienteListByTutor(PGconn *conn, const Sesion *sesion, const char *tutorRut);
PGresult *pacienteCreate(PGconn *conn, const Sesion *sesion, const Paciente *paciente);
PGresult *pacienteUpdate(PGconn *conn, const Sesion *sesion, const char *id, const Paciente *paciente);
PGresult *pacienteDelete(PGconn *conn, const Sesion *sesion, const char *id);

#endif
