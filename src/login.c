#include <stdlib.h>
#include "login.h"

#include <libpq-fe.h>
#include <argon2.h>
#include <string.h>

Sesion *login(PGconn *conn, const char *rut, const char *passwd)
{
    if (conn == NULL || rut == NULL || passwd == NULL)
        return NULL;

    Sesion *sesion = calloc(1, sizeof(*sesion));
    if (sesion == NULL)
        return NULL;

    if (strlen(rut) >= sizeof(sesion->rut)) {
        free(sesion);
        return NULL;
    }

    strcpy(sesion->rut, rut);   
    /* La contraseña se usa solo para verificar el hash, no se guarda en la sesión. */
    const char *paramValues[1] = { sesion->rut };

    PGresult *res = PQexecParams(
        conn,
        "SELECT passwd FROM public.usuarios WHERE fk_doctor = $1",
        1,
        NULL,
        paramValues,
        NULL,
        NULL,
        0
    );

    if (res == NULL || PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        if (res != NULL)
            PQclear(res);

        free(sesion);
        return NULL;
    }

    if (PQntuples(res) != 1)
    {
        PQclear(res);
        free(sesion);
        return NULL;
    }

    const char *hash = PQgetvalue(res, 0, 0);

    int resultado = argon2id_verify(
        hash,
        passwd,
        strlen(passwd)
    );

    PQclear(res);

    if (resultado != ARGON2_OK)
    {
        free(sesion);
        return NULL;
    }

    memset(sesion->psswd, 0, sizeof(sesion->psswd));

    return sesion;
} 
