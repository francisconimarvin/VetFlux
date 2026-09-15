#include <stdio.h>
#include <string.h>
#include <libpq-fe.h>
#include "tutor.h"
#include "utils.h"

// Create tutor
void create_Tutor(PGconn *conn) {
    Tutor tutor;

    printf("RUT: ");
    input_string(tutor.rut, sizeof(tutor.rut));
    
    printf("Nombre: ");
    input_string(tutor.nombre, sizeof(tutor.nombre));
    
    printf("Teléfono: ");
    input_string(tutor.telefono, sizeof(tutor.telefono));

    printf("Email: ");
    input_string(tutor.email, sizeof(tutor.email));

    printf("Domicilio: ");
    input_string(tutor.domicilio, sizeof(tutor.domicilio));

    // Crear arreglos con los datos obtenidos
    const char *values[] = {
        tutor.rut,
        tutor.nombre,
        tutor.telefono,
        tutor.email,
        tutor.domicilio
    };

    // SQL Insert
    PGresult *insert = PQexecParams(
        conn,
        "INSERT INTO tutor (rut, nombre, telefono, email, domicilio) "
        "VALUES ($1, $2, $3, $4, $5)",
        5,
        NULL,
        values,
        NULL,
        NULL,
        0
    );

    if (PQresultStatus(insert) == PGRES_COMMAND_OK) {
    printf("Tutor creado correctamente.\n");
    }
    else {
    fprintf(stderr, "Error al crear tutor: %s\n", PQerrorMessage(conn));
    }

    PQclear(insert);
}

// Delete tutor (USING PK)
void delete_Tutor(PGconn *conn) {
    Tutor tutor;

    printf("RUT de tutor a eliminar: ");
    input_string(tutor.rut, sizeof(tutor.rut));
    
    const char *rut[] = {tutor.rut};

    PGresult *delete = PQexecParams(
        conn,
        "DELETE FROM tutor "
        "   WHERE rut = $1",
        1,
        NULL,
        rut,
        NULL,
        NULL,
        0
    );
    
    if (PQresultStatus(delete) == PGRES_COMMAND_OK) {
        printf("Tutor borrado correctamente.\n");
    }
    else {
        fprintf(stderr, "Error al borrar tutor: %s\n", PQerrorMessage(conn));

    }

    PQclear(delete);
}

// Read tutor (USING PK)
void read_Tutor(PGconn *conn) {
    Tutor tutor;

    printf("RUT del tutor a buscar: ");
    input_string(tutor.rut, sizeof(tutor.rut));
    
    const char *rut[] = {tutor.rut};

    PGresult *read = PQexecParams(
            conn,
            "SELECT * FROM tutor " 
                "WHERE rut = $1",
            1,
            NULL, 
            rut,
            NULL,
            NULL,
            0
    );
    if (PQresultStatus(read) == PGRES_TUPLES_OK) {
    printf("Información del tutor:\n");

    int rows = PQntuples(read);
    int cols = PQnfields(read);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%s: %s\n",
                   PQfname(read, j),
                   PQgetvalue(read, i, j));
        }
    }
} else {
    fprintf(stderr, "Error al leer el tutor: %s\n", PQerrorMessage(conn));
}
    PQclear(read);

}

// Update tutor
void update_Tutor(PGconn *conn)
{
    Tutor tutor;
    char rut[13];
    int option;

    printf("RUT del tutor a modificar: ");
    input_string(rut, sizeof(rut));

    printf("\nElija una opción a modificar:\n");
    printf("1. Nombre\n");
    printf("2. Teléfono\n");
    printf("3. Email\n");
    printf("4. Domicilio\n");
    printf("5. Cancelar\n");

    printf("Opción: ");
    scanf("%d", &option);

    switch (option)
    {
        case 1:
            printf("Modificar nombre: ");
            input_string(tutor.nombre, sizeof(tutor.nombre));

            const char *values[] = {
                tutor.nombre,
                rut
            };

            PGresult *update = PQexecParams(
                conn,
                "UPDATE tutor SET nombre = $1 WHERE rut = $2",
                2,
                NULL,
                values,
                NULL,
                NULL,
                0
            );

            PQclear(update);
            break;

        // case 2...
        // case 3...
        // case 4...

        case 5:
            return;

        default:
            printf("Opción inválida.\n");
            break;
        
    }
}
