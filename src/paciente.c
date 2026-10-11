#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include "paciente.h"
#include "db.h"
#include "login.h"
#include "utils.h"

void createPaciente(PGconn *conn, const Sesion *sesion)
{
    Paciente paciente;
    char edad[12];

    printf("Nombre del paciente:\n");
    fgets(paciente.nombre, sizeof(paciente.nombre), stdin);
    paciente.nombre[strcspn(paciente.nombre, "\n")] = '\0';

    printf("Edad del paciente:\n");
    fgets(edad, sizeof(edad), stdin);
    edad[strcspn(edad, "\n")] = '\0';

    printf("Sexo del paciente (H/M):\n");
    fgets(paciente.sexo, sizeof(paciente.sexo), stdin);
    paciente.sexo[strcspn(paciente.sexo, "\n")] = '\0';

    printf("Especie del paciente:\n");
    fgets(paciente.especie, sizeof(paciente.especie), stdin);
    paciente.especie[strcspn(paciente.especie, "\n")] = '\0';

    printf("Raza del paciente:\n");
    fgets(paciente.raza, sizeof(paciente.raza), stdin);
    paciente.raza[strcspn(paciente.raza, "\n")] = '\0';

    printf("Color del paciente:\n");
    fgets(paciente.color, sizeof(paciente.color), stdin);
    paciente.color[strcspn(paciente.color, "\n")] = '\0';

    printf("RUT del tutor:\n");
    fgets(paciente.tutor_fk, sizeof(paciente.tutor_fk), stdin);
    paciente.tutor_fk[strcspn(paciente.tutor_fk, "\n")] = '\0';

    const char *paramValues[8] = {
        paciente.nombre,
        edad,
        paciente.sexo,
        paciente.especie,
        paciente.raza,
        paciente.color,
        paciente.tutor_fk,
        sesion->rut
    };

    PGresult *res = PQexecParams(
            conn,
            "INSERT INTO public.paciente "
            "(nombre, edad, sexo, especie, raza, color, tutor_fk, doctor_fk) "
            "VALUES ($1, $2::integer, $3, $4, $5, $6, $7, $8)",
            8,
            NULL,
            paramValues,
            NULL,
            NULL,
            0
            );

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "INSERT failed: %s\n", PQerrorMessage(conn));
    }
    else
    {
        printf("Paciente creado exitosamente\n");
    }

    PQclear(res);
}

void readAllPaciente(PGconn *conn, const Sesion *sesion)
{
    const char *paramValues[1] = { sesion->rut };

    PGresult *res = PQexecParams(
            conn,
            "SELECT * FROM public.paciente "
            "WHERE doctor_fk = $1 "
            "ORDER BY id",
            1,
            NULL,
            paramValues,
            NULL,
            NULL,
            0
            );

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    int columns = PQnfields(res);

    if (rows == 0)
    {
        printf("No hay pacientes registrados.\n");
        PQclear(res);
        return;
    }

    for (int j = 0; j < columns; j++)
    {
        printf("%-16s", PQfname(res, j));
    }

    printf("\n");

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%-16s", PQgetvalue(res, i, j));
        }

        printf("\n");
    }

    PQclear(res);
}

void readPaciente(PGconn *conn, const Sesion *sesion)
{
    printf("¿Cómo quieres buscar al paciente?\n");
    printf("1. Por ID\n");
    printf("2. Por RUT del tutor\n");

    int option = getInteger();

    if (option == -1)
    {
        printf("Opción inválida.\n");
        return;
    }

    char search[100];
    const char *query;
    const char *paramValues[2];

    switch (option)
    {
        case 1:
            printf("ID del paciente:\n");
            fgets(search, sizeof(search), stdin);
            search[strcspn(search, "\n")] = '\0';

            query =
                "SELECT * FROM public.paciente "
                "WHERE id = $1::integer AND doctor_fk = $2";

            break;

        case 2:
            printf("RUT del tutor:\n");
            fgets(search, sizeof(search), stdin);
            search[strcspn(search, "\n")] = '\0';

            query =
                "SELECT * FROM public.paciente "
                "WHERE tutor_fk = $1 AND doctor_fk = $2 "
                "ORDER BY id";

            break;

        default:
            printf("Opción inválida\n");
            return;
    }

    paramValues[0] = search;
    paramValues[1] = sesion->rut;

    PGresult *res = PQexecParams(
            conn,
            query,
            2,
            NULL,
            paramValues,
            NULL,
            NULL,
            0
            );

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    int columns = PQnfields(res);

    if (rows == 0)
    {
        printf("No se encontraron pacientes.\n");
        PQclear(res);
        return;
    }

    for (int j = 0; j < columns; j++)
    {
        printf("%-16s", PQfname(res, j));
    }

    printf("\n");

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%-16s", PQgetvalue(res, i, j));
        }

        printf("\n");
    }

    PQclear(res);
}

void updatePaciente(PGconn *conn, const Sesion *sesion)
{
    char id[12];
    char value[200];

    printf("ID del paciente que quieres actualizar:\n");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';

    printf("¿Qué quieres actualizar del paciente?\n");
    printf("1. Nombre\n");
    printf("2. Edad\n");
    printf("3. Sexo\n");
    printf("4. Especie\n");
    printf("5. Raza\n");
    printf("6. Color\n");
    printf("7. RUT del tutor\n");

    int option;

    if (scanf("%d", &option) != 1)
    {
        printf("Opción inválida.\n");
        while (getchar() != '\n');
        return;
    }

    getchar();

    const char *query;
    const char *field;

    switch (option)
    {
        case 1:
            field = "nombre";
            query =
                "UPDATE public.paciente SET nombre = $1 "
                "WHERE id = $2::integer AND doctor_fk = $3";
            printf("Nuevo nombre:\n");
            break;

        case 2:
            field = "edad";
            query =
                "UPDATE public.paciente SET edad = $1::integer "
                "WHERE id = $2::integer AND doctor_fk = $3";
            printf("Nueva edad:\n");
            break;

        case 3:
            field = "sexo";
            query =
                "UPDATE public.paciente SET sexo = $1 "
                "WHERE id = $2::integer AND doctor_fk = $3";
            printf("Nuevo sexo (H/M):\n");
            break;

        case 4:
            field = "especie";
            query =
                "UPDATE public.paciente SET especie = $1 "
                "WHERE id = $2::integer AND doctor_fk = $3";
            printf("Nueva especie:\n");
            break;

        case 5:
            field = "raza";
            query =
                "UPDATE public.paciente SET raza = $1 "
                "WHERE id = $2::integer AND doctor_fk = $3";
            printf("Nueva raza:\n");
            break;

        case 6:
            field = "color";
            query =
                "UPDATE public.paciente SET color = $1 "
                "WHERE id = $2::integer AND doctor_fk = $3";
            printf("Nuevo color:\n");
            break;

        case 7:
            field = "tutor_fk";
            query =
                "UPDATE public.paciente SET tutor_fk = $1 "
                "WHERE id = $2::integer AND doctor_fk = $3";
            printf("Nuevo RUT del tutor:\n");
            break;

        default:
            printf("Opción inválida\n");
            return;
    }

    fgets(value, sizeof(value), stdin);
    value[strcspn(value, "\n")] = '\0';

    const char *paramValues[3] = {
        value,
        id,
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
    else if (atoi(PQcmdTuples(res)) == 0)
    {
        printf("No existe un paciente con ese ID o no tienes acceso a él.\n");
    }
    else
    {
        printf("Paciente: %s modificado exitosamente.\n", field);
    }

    PQclear(res);
}

void deletePaciente(PGconn *conn, const Sesion *sesion)
{
    char id[12];

    printf("ID del paciente a eliminar:\n");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';

    const char *paramValues[2] = {
        id,
        sesion->rut
    };

    PGresult *res = PQexecParams(
            conn,
            "DELETE FROM public.paciente "
            "WHERE id = $1::integer AND doctor_fk = $2 "
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
        printf("No existe un paciente con ese ID o no tienes acceso a él.\n");
        PQclear(res);
        return;
    }

    printf("\nPaciente eliminado:\n");

    for (int i = 0; i < PQnfields(res); i++)
    {
        printf("%s: %s\n",
                PQfname(res, i),
                PQgetvalue(res, 0, i));
    }

    PQclear(res);
}
