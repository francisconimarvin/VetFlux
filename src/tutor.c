#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include "tutor.h"
#include "login.h"
#include "db.h"
#include "utils.h"

void createTutor(PGconn *conn, const Sesion *sesion)
{
    Tutor tutor;

    printf("RUT del tutor: (ej 21.111.111-2)\n");
    fgets(tutor.rut, sizeof(tutor.rut), stdin);
    tutor.rut[strcspn(tutor.rut, "\n")] = 0;

    printf("Nombre del tutor:\n");
    fgets(tutor.nombre, sizeof(tutor.nombre), stdin);
    tutor.nombre[strcspn(tutor.nombre, "\n")] = 0;

    printf("eMail del tutor:\n");
    fgets(tutor.email, sizeof(tutor.email), stdin);
    tutor.email[strcspn(tutor.email, "\n")] = 0;

    printf("Teléfono del tutor (ej 911112233):\n");
    fgets(tutor.telefono, sizeof(tutor.telefono), stdin);
    tutor.telefono[strcspn(tutor.telefono, "\n")] = 0;

    printf("Domicilio del tutor:\n");
    fgets(tutor.domicilio, sizeof(tutor.domicilio), stdin);
    tutor.domicilio[strcspn(tutor.domicilio, "\n")] = 0;

    const char *paramValues[6] = {
        tutor.rut,
        tutor.nombre,
        tutor.email,
        tutor.telefono,
        tutor.domicilio,
        sesion->rut
    };

    PGresult *res = PQexecParams(
            conn,
            "INSERT INTO public.tutor "
            "(rut, nombre, email, telefono, domicilio, doctor_fk) "
            "VALUES ($1, $2, $3, $4, $5, $6)",
            6,
            NULL,
            paramValues,
            NULL,
            NULL,
            0
            );

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "INSERT failed: %s", PQerrorMessage(conn));
    }
    else
    {
        printf("Tutor creado exitosamente\n");
    }

    PQclear(res);
}

void readAllTutor(PGconn *conn, const Sesion *sesion)
{
    const char *paramValues[1] = {
        sesion->rut
    };

    PGresult *res = PQexecParams(
            conn,
            "SELECT * FROM public.tutor "
            "WHERE doctor_fk = $1 "
            "ORDER BY rut",
            1,
            NULL,
            paramValues,
            NULL,
            NULL,
            0
            );

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    int columns = PQnfields(res);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%s: %s ",
                    PQfname(res, j),
                    PQgetvalue(res, i, j));
        }

        printf("\n");
    }

    if (rows == 0)
    {
        printf("No tienes tutores registrados.\n");
    }

    PQclear(res);
}

void readTutor(PGconn *conn, const Sesion *sesion)
{
    Tutor tutor;

    printf("\nRUT del tutor a buscar:\n");
    fgets(tutor.rut, sizeof(tutor.rut), stdin);
    tutor.rut[strcspn(tutor.rut, "\n")] = 0;

    const char *paramValues[2] = {
        tutor.rut,
        sesion->rut
    };

    PGresult *res = PQexecParams(
            conn,
            "SELECT * FROM public.tutor "
            "WHERE rut = $1 AND doctor_fk = $2",
            2,
            NULL,
            paramValues,
            NULL,
            NULL,
            0
            );

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    int columns = PQnfields(res);

    if (rows == 0)
    {
        printf("No existe un tutor con ese RUT en tu clínica.\n");
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%s: %s\n",
                    PQfname(res, j),
                    PQgetvalue(res, i, j));
        }
    }

    PQclear(res);
}

void updateTutor(PGconn *conn, const Sesion *sesion)
{
    Tutor tutor;

    printf("¿Qué quieres actualizar del tutor?\n");
    printf("1. Nombre\n");
    printf("2. eMail\n");
    printf("3. Teléfono\n");
    printf("4. Domicilio\n");
    printf("Selecciona una opción: ");

    int option = getInteger();

    if (option == -1)
    {
        printf("Opción inválida.\n");
        return;
    }

    const char *campo = NULL;
    const char *valor = NULL;

    switch (option)
    {
        case 1:
            campo = "nombre";
            printf("RUT del tutor a cambiar el nombre:\n");
            break;

        case 2:
            campo = "email";
            printf("RUT del tutor a cambiar el eMail:\n");
            break;

        case 3:
            campo = "telefono";
            printf("RUT del tutor a cambiar el teléfono:\n");
            break;

        case 4:
            campo = "domicilio";
            printf("RUT del tutor a cambiar el domicilio:\n");
            break;

        default:
            printf("Opción inválida.\n");
            return;
    }

    fgets(tutor.rut, sizeof(tutor.rut), stdin);
    tutor.rut[strcspn(tutor.rut, "\n")] = '\0';

    switch (option)
    {
        case 1:
            printf("Nuevo nombre:\n");
            fgets(tutor.nombre, sizeof(tutor.nombre), stdin);
            tutor.nombre[strcspn(tutor.nombre, "\n")] = '\0';
            valor = tutor.nombre;
            break;

        case 2:
            printf("Nuevo eMail:\n");
            fgets(tutor.email, sizeof(tutor.email), stdin);
            tutor.email[strcspn(tutor.email, "\n")] = '\0';
            valor = tutor.email;
            break;

        case 3:
            printf("Nuevo teléfono:\n");
            fgets(tutor.telefono, sizeof(tutor.telefono), stdin);
            tutor.telefono[strcspn(tutor.telefono, "\n")] = '\0';
            valor = tutor.telefono;
            break;

        case 4:
            printf("Nuevo domicilio:\n");
            fgets(tutor.domicilio, sizeof(tutor.domicilio), stdin);
            tutor.domicilio[strcspn(tutor.domicilio, "\n")] = '\0';
            valor = tutor.domicilio;
            break;
    }

    /*
     * El nombre de la columna se selecciona mediante el switch.
     * Los valores ingresados se envían como parámetros.
     */
    char query[200];

    snprintf(
            query,
            sizeof(query),
            "UPDATE public.tutor SET %s = $1 "
            "WHERE rut = $2 AND doctor_fk = $3",
            campo
            );

    const char *paramValues[3] = {
        valor,
        tutor.rut,
        sesion->rut
    };

    PGresult *res = PQexecParams(
            conn,
            query,
            3,
            NULL,
            paramValues,
            NULL,
            NULL,
            0
            );

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "UPDATE failed: %s\n", PQerrorMessage(conn));
    }
    else if (strcmp(PQcmdTuples(res), "0") == 0)
    {
        printf("No existe un tutor con ese RUT en tu clínica.\n");
    }
    else
    {
        printf("Tutor modificado exitosamente.\n");
    }

    PQclear(res);
}

void deleteTutor(PGconn *conn, const Sesion *sesion)
{
    Tutor tutor;

    printf("RUT del tutor a eliminar:\n");
    fgets(tutor.rut, sizeof(tutor.rut), stdin);
    tutor.rut[strcspn(tutor.rut, "\n")] = '\0';

    const char *paramValues[2] = {
        tutor.rut,
        sesion->rut
    };

    PGresult *res = PQexecParams(
            conn,
            "DELETE FROM public.tutor "
            "WHERE rut = $1 AND doctor_fk = $2 "
            "RETURNING *",
            2,
            NULL,
            paramValues,
            NULL,
            NULL,
            0
            );

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "DELETE failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    if (PQntuples(res) == 0)
    {
        printf("No existe un tutor con ese RUT en tu clínica.\n");
        PQclear(res);
        return;
    }

    printf("\nTutor eliminado:\n");

    for (int i = 0; i < PQnfields(res); i++)
    {
        printf("%s: %s\n",
                PQfname(res, i),
                PQgetvalue(res, 0, i));
    }

    PQclear(res);
}

