#include <stdio.h>
#include <stdlib.h>
#include "menu.h"
#include "tutor.h"
#include "utils.h"

void mostrarMenu(PGconn *conn)
{
    int opcion = -1;

    do {
        printf("\n=== Bienvenido a VetFlux ===\n");
        printf("1. Administrar tutores\n");
        printf("8. Terminar y salir\n");
        printf("Selecciona una opcion: ");

        opcion = getInteger();

        if (opcion == -1) {
            printf("Entrada inválida.\n");
            continue;
        }

        switch (opcion) {
            case 1: {
                int opcionCrud = -1;

                do {
                    printf("\n=== Administrar tutores ===\n");
                    printf("1. Crear tutor\n");
                    printf("2. Actualizar tutor\n");
                    printf("3. Mostrar todos los tutores\n");
                    printf("4. Mostrar tutor por RUT\n");
                    printf("5. Borrar tutor por RUT\n");
                    printf("6. Volver al menu principal\n");
                    printf("Selecciona una opcion: ");

                    opcionCrud = getInteger();

                    if (opcionCrud == -1) {
                        printf("Entrada inválida.\n");
                        continue;
                    }

                    switch (opcionCrud) {
                        case 1:
                            createTutor(conn);
                            break;

                        case 2:
                            updateTutor(conn);
                            break;

                        case 3:
                            readAllTutor(conn);
                            break;

                        case 4:
                            readTutor(conn);
                            break;

                        case 5:
                            deleteTutor(conn);
                            break;

                        case 6:
                            printf("Volviendo al menu principal...\n");
                            break;

                        default:
                            printf("Opcion invalida.\n");
                            break;
                    }

                } while (opcionCrud != 6);

                break;
            }

            case 8:
                printf("Saliendo de VetFlux...\n");
                break;

            default:
                printf("Opcion invalida.\n");
                break;
        }

    } while (opcion != 8);
}
