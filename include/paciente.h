#ifndef PACIENTE_H
#define PACIENTE_H

#include <stdio.h>
#include <libpq-fe.h>

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

void createPaciente(PGconn *conn);
void readAllPaciente(PGconn *conn);
void readPaciente(PGconn *conn);
void updatePaciente(PGconn *conn);
void deletePaciente(PGconn *conn);

#endif

