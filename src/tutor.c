#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include "tutor.h"
#include "db.h"

void createTutor(PGconn *conn)
{
    Tutor tutor;
    printf("RUT del tutor: (ej 21.111.111-2)\n");
    fgets(tutor.rut, sizeof(tutor.rut), stdin);
    tutor.rut[strcspn(tutor.rut,"\n")] = 0;

    printf("Nombre del tutor:\n");
    fgets(tutor.nombre, sizeof(tutor.nombre), stdin);
    tutor.nombre[strcspn(tutor.nombre,"\n")] = 0;

    printf("eMail del tutor:\n");
    fgets(tutor.email, sizeof(tutor.email), stdin);
    tutor.email[strcspn(tutor.email,"\n")] = 0;

    printf("Teléfono del tutor (ej 911112233):\n");
    fgets(tutor.telefono, sizeof(tutor.telefono), stdin);
    tutor.telefono[strcspn(tutor.telefono,"\n")] = 0;

    printf("Domicilio del tutor:\n");
    fgets(tutor.domicilio, sizeof(tutor.domicilio), stdin);
    tutor.domicilio[strcspn(tutor.domicilio,"\n")] = 0;

    const char *paramValues[5] = {
        tutor.rut,
        tutor.nombre,
        tutor.email,
        tutor.telefono,
        tutor.domicilio
    };
   
    PGresult *res;
    // int paramFormats[] = { 0 };
    res = PQexecParams(
            conn,
            "INSERT INTO public.tutor VALUES "
            "($1::varchar, $2::varchar, $3::varchar, $4::varchar, $5::varchar)",
            5,
            NULL,
            paramValues,
            NULL,
            NULL,
            0
            );

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "INSERT failed: %s", PQerrorMessage(conn));
    } else {
        printf("Tutor creado exitosamente\n");
    }
    PQclear(res);
   
}

void readAllTutor(PGconn *conn) 
{
    Tutor tutor;
    
    // Select * tutor
    PGresult *res;
    res = PQexecParams(
            conn, 
            "SELECT * FROM public.tutor",
            0,
            NULL,
            NULL,
            NULL,
            NULL,
            0
            );

   if (PQresultStatus(res) != PGRES_TUPLES_OK)
   {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
   }

   int rows = PQntuples(res);
   int columns = PQnfields(res);

   for (int i = 0; i < rows; i++)
   {
       for (int j = 0; j < columns; j++)
       {
           printf("%s ", PQgetvalue(res, i, j));
       }
   }

   PQclear(res);

}

// I should create a function for this on utils.h maybe haha
void readTutor(PGconn *conn)
{
    Tutor tutor;
    printf("\nRUT del tutor a buscar: \n");
    fgets(tutor.rut, sizeof(tutor.rut), stdin);
    tutor.rut[strcspn(tutor.rut,"\n")] = 0;

    const char *paramValues[1] = {tutor.rut};
    PGresult *res;
    res = PQexecParams(
            conn, 
            "SELECT * FROM public.tutor "
            "WHERE rut=$1",
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
    }

    int rows = PQntuples(res);
    int columns = PQnfields(res);

    for (int i = 0; i < rows; i++)
    {
       for (int j = 0; j < columns; j++)
       {
           printf("%s ", PQgetvalue(res, i, j));
       }
       printf("\n");
    }
    PQclear(res);
}

void updateTutor(PGconn *conn)
{
    Tutor tutor;

    printf("¿Qué quieres actualizar del tutor?\n");
    printf("1. Nombre\n");
    printf("2. eMail\n");
    printf("3. Teléfono\n");
    printf("4. Domicilio\n");

    int option;
    scanf("%d", &option);

    getchar();
    switch (option)
    {
        case 1:
        {
            printf("RUT del tutor a cambiar el nombre:\n");
            fgets(tutor.rut, sizeof(tutor.rut), stdin);
            tutor.rut[strcspn(tutor.rut, "\n")] = '\0';

            printf("Nuevo nombre:\n");
            fgets(tutor.nombre, sizeof(tutor.nombre), stdin);
            tutor.nombre[strcspn(tutor.nombre, "\n")] = '\0';

            const char *paramValues[2] = {
                tutor.nombre,
                tutor.rut
            };

            PGresult *res = PQexecParams(
                conn,
                "UPDATE public.tutor "
                "SET nombre = $1 "
                "WHERE rut = $2",
                2,
                NULL,
                paramValues,
                NULL,
                NULL,
                0
            );

            if (PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                fprintf(stderr,
                        "UPDATE failed: %s\n",
                        PQerrorMessage(conn));
            }
            else
            {
                printf("Tutor NOMBRE modificado exitosamente\n");
            }

            PQclear(res);

            break;
        }

        case 2:
        {
            printf("RUT del tutor a cambiar el eMail:\n");
            fgets(tutor.rut, sizeof(tutor.rut), stdin);
            tutor.rut[strcspn(tutor.rut, "\n")] = '\0';

            printf("Nuevo eMail:\n");
            fgets(tutor.email, sizeof(tutor.email), stdin);
            tutor.email[strcspn(tutor.email, "\n")] = '\0';

            const char *paramValues[2] = {
                tutor.email,
                tutor.rut
            };

            PGresult *res = PQexecParams(
                conn,
                "UPDATE public.tutor "
                "SET email = $1 "
                "WHERE rut = $2",
                2,
                NULL,
                paramValues,
                NULL,
                NULL,
                0
            );

            if (PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                fprintf(stderr,
                        "UPDATE failed: %s\n",
                        PQerrorMessage(conn));
            }
            else
            {
                printf("Tutor EMAIL modificado exitosamente\n");
            }

            PQclear(res);

            break;
        }

        case 3:
        {
            printf("RUT del tutor a cambiar el teléfono:\n");
            fgets(tutor.rut, sizeof(tutor.rut), stdin);
            tutor.rut[strcspn(tutor.rut, "\n")] = '\0';

            printf("Nuevo teléfono:\n");
            fgets(tutor.telefono, sizeof(tutor.telefono), stdin);
            tutor.telefono[strcspn(tutor.telefono, "\n")] = '\0';

            const char *paramValues[2] = {
                tutor.telefono,
                tutor.rut
            };

            PGresult *res = PQexecParams(
                conn,
                "UPDATE public.tutor "
                "SET telefono = $1 "
                "WHERE rut = $2",
                2,
                NULL,
                paramValues,
                NULL,
                NULL,
                0
            );

            if (PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                fprintf(stderr,
                        "UPDATE failed: %s\n",
                        PQerrorMessage(conn));
            }
            else
            {
                printf("Tutor TELÉFONO modificado exitosamente\n");
            }

            PQclear(res);

            break;
        }

        case 4:
        {
            printf("RUT del tutor a cambiar el domicilio:\n");
            fgets(tutor.rut, sizeof(tutor.rut), stdin);
            tutor.rut[strcspn(tutor.rut, "\n")] = '\0';

            printf("Nuevo domicilio:\n");
            fgets(tutor.domicilio, sizeof(tutor.domicilio), stdin);
            tutor.domicilio[strcspn(tutor.domicilio, "\n")] = '\0';

            const char *paramValues[2] = {
                tutor.domicilio,
                tutor.rut
            };

            PGresult *res = PQexecParams(
                conn,
                "UPDATE public.tutor "
                "SET domicilio = $1 "
                "WHERE rut = $2",
                2,
                NULL,
                paramValues,
                NULL,
                NULL,
                0
            );

            if (PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                fprintf(stderr,
                        "UPDATE failed: %s\n",
                        PQerrorMessage(conn));
            }
            else
            {
                printf("Tutor DOMICILIO modificado exitosamente\n");
            }

            PQclear(res);

            break;
        }

        default:
            printf("Opción inválida\n");
            break;
    }
}

void deleteTutor(PGconn *conn)
{
    Tutor tutor;

    printf("RUT del tutor a eliminar:\n");
    fgets(tutor.rut, sizeof(tutor.rut), stdin);
    tutor.rut[strcspn(tutor.rut, "\n")] = '\0';

    const char *paramValues[1] = {
        tutor.rut
    };

    PGresult *res = PQexecParams(
        conn,
        "DELETE FROM public.tutor "
        "WHERE rut = $1 "
        "RETURNING *",
        1,
        NULL,
        paramValues,
        NULL,
        NULL,
        0
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr,
                "DELETE failed: %s\n",
                PQerrorMessage(conn));

        PQclear(res);
        return;
    }

    if (PQntuples(res) == 0)
    {
        printf("No existe un tutor con ese RUT.\n");
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
