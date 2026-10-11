#ifndef PACIENTE_H
#define PACIENTE_H

#include <stdio.h>
#include <libpq-fe.h>
#include "login.h"

typedef struct paciente 
{
    int id;
    char nombre[100];
    int edad;
    char sexo[2];
    char especie[50];
    char raza[100];
    char color[50];
    char tutor_fk[16];
    char doctor_fk[16];
} Paciente;

void createPaciente(PGconn *conn, const Sesion *sesion);
void readAllPaciente(PGconn *conn, const Sesion *sesion);
void readPaciente(PGconn *conn, const Sesion *sesion);
void updatePaciente(PGconn *conn, const Sesion *sesion);
void deletePaciente(PGconn *conn, const Sesion *sesion);

#endif

