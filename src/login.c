#include <stdio.h>
#include <stdlib.h>
#include "login.h"

#include <libpq-fe.h>
#include <argon2.h> // install argon2 on your local machine
#include <string.h>

Sesion *login(PGconn *conn)
{
    Sesion *sesion = malloc(sizeof(Sesion));

    if (sesion == NULL)
        return NULL;

    printf("Doctor, ingrese su rut: (ej 21.111.111-2)\n");
    fgets(sesion->rut, sizeof(sesion->rut), stdin);
    sesion->rut[strcspn(sesion->rut, "\n")] = 0;

    printf("Doctor, ingrese su contraseña:\n");
    fgets(sesion->psswd, sizeof(sesion->psswd), stdin);
    sesion->psswd[strcspn(sesion->psswd, "\n")] = 0;

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
        fprintf(stderr, "Error al consultar usuario: %s\n", PQerrorMessage(conn));

        if (res != NULL)
            PQclear(res);

        free(sesion);
        return NULL;
    }

    if (PQntuples(res) != 1)
    {
        printf("RUT o contraseña incorrectos.\n");
        PQclear(res);
        free(sesion);
        return NULL;
    }

    // Read the documentation in https://github.com/winlibs/argon2/blob/master/include/argon2.h#L343
    const char *hash = PQgetvalue(res, 0, 0);

    int resultado = argon2id_verify(
            hash,
            sesion->psswd,
            strlen(sesion->psswd)
            );

    PQclear(res);

    if (resultado != ARGON2_OK)
    {
        printf("RUT o contraseña incorrectos.\n");
        free(sesion);
        return NULL;
    }

    memset(sesion->psswd, 0, sizeof(sesion->psswd));

    printf("Inicio de sesión exitoso.\n");
    return sesion;
}
