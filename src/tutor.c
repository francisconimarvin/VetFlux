#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "tutor.h"
#include "db.h"

void createTutor(PGconn *conn)
{
    Tutor tutor;
    printf("RUT del tutor: (ej 26.051.367-2)\n");
    scanf("%[^\n]%*c", tutor.rut);

    printf("Nombre del tutor:\n");
    scanf("%[^\n]%*c", tutor.nombre);

    printf("eMail del tutor:\n");
    scanf("%[^\n]%*c", tutor.email);

    printf("Teléfono del tutor (ej 911112233):\n");
    scanf("%[^\n]%*c", tutor.telefono);

    printf("Domicilio del tutor:\n");
    scanf("%[^\n]%*c", tutor.domicilio);    

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
