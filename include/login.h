#ifndef LOGIN_H
#define LOGIN_H

#include <libpq-fe.h>

typedef struct sesion 
{
    char rut[16];
    char psswd[258];
    int success;
} Sesion;

int login(PGconn *conn);

#endif
