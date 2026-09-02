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
    
    const char *rut[] = {
        tutor.rut
    };

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
