#include <stdio.h>
#include <stdlib.h>

#include "menu.h"
#include "tutor.h"
#include "paciente.h"
#include "agenda.h"
#include "utils.h"
#include "login.h"
#include "inventario.h"
#include "fichaMedica.h"
#include "receta.h"

void mostrarMenu(PGconn *conn, const Sesion *sesion)
{
    int opcion = -1;

    do
    {
        printf("\n=== Bienvenido a VetFlux ===\n");
        printf("1. Administrar tutores\n");
        printf("2. Administrar pacientes\n");
        printf("3. Administrar agendas\n");
        printf("4. Administrar inventario\n");
        printf("5. Administrar fichas medicas\n");
        printf("6. Administrar recetas\n");
        printf("7. Terminar y salir\n");
        printf("Selecciona una opcion: ");

        opcion = getInteger();

        if (opcion == -1)
        {
            printf("Entrada invalida.\n");
            continue;
        }

        switch (opcion)
        {
        case 1:
        {
            int opcionCrud = -1;

            do
            {
                printf("\n=== Administrar tutores ===\n");
                printf("1. Crear tutor\n");
                printf("2. Actualizar tutor\n");
                printf("3. Mostrar todos los tutores\n");
                printf("4. Mostrar tutor por RUT\n");
                printf("5. Borrar tutor por RUT\n");
                printf("6. Volver al menu principal\n");
                printf("Selecciona una opcion: ");

                opcionCrud = getInteger();

                if (opcionCrud == -1)
                {
                    printf("Entrada invalida.\n");
                    continue;
                }

                switch (opcionCrud)
                {
                case 1:
                    createTutor(conn, sesion);
                    break;

                case 2:
                    updateTutor(conn, sesion);
                    break;

                case 3:
                    readAllTutor(conn, sesion);
                    break;

                case 4:
                    readTutor(conn, sesion);
                    break;

                case 5:
                    deleteTutor(conn, sesion);
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

        case 2:
        {
            int opcionCrud = -1;

            do
            {
                printf("\n=== Administrar pacientes ===\n");
                printf("1. Crear paciente\n");
                printf("2. Actualizar paciente\n");
                printf("3. Mostrar todos los pacientes\n");
                printf("4. Mostrar paciente por ID o RUT del tutor\n");
                printf("5. Borrar paciente por ID\n");
                printf("6. Volver al menu principal\n");
                printf("Selecciona una opcion: ");

                opcionCrud = getInteger();

                if (opcionCrud == -1)
                {
                    printf("Entrada invalida.\n");
                    continue;
                }

                switch (opcionCrud)
                {
                case 1:
                    createPaciente(conn, sesion);
                    break;

                case 2:
                    updatePaciente(conn, sesion);
                    break;

                case 3:
                    readAllPaciente(conn, sesion);
                    break;

                case 4:
                    readPaciente(conn, sesion);
                    break;

                case 5:
                    deletePaciente(conn, sesion);
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

        case 3:
        {
            int opcionCrud = -1;

            do
            {
                printf("\n=== Administrar agendas ===\n");
                printf("1. Crear agenda\n");
                printf("2. Actualizar agenda\n");
                printf("3. Mostrar todas las agendas\n");
                printf("4. Mostrar agendas por paciente\n");
                printf("5. Borrar agenda por ID\n");
                printf("6. Volver al menu principal\n");
                printf("Selecciona una opcion: ");

                opcionCrud = getInteger();

                if (opcionCrud == -1)
                {
                    printf("Entrada invalida.\n");
                    continue;
                }

                switch (opcionCrud)
                {
                case 1:
                    createAgenda(conn, sesion);
                    break;

                case 2:
                    updateAgenda(conn, sesion);
                    break;

                case 3:
                    readAllAgenda(conn, sesion);
                    break;

                case 4:
                    readAgenda(conn, sesion);
                    break;

                case 5:
                    deleteAgenda(conn, sesion);
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

        case 4:
        {
            int opcionCrud = -1;

            do
            {
                printf("\n=== Administrar inventario ===\n");
                printf("1. Mostrar inventario por ID de instrumento\n");
                printf("2. Actualizar cantidad de instrumento\n");
                printf("3. Volver al menu principal\n");
                printf("Selecciona una opcion: ");

                opcionCrud = getInteger();

                if (opcionCrud == -1)
                {
                    printf("Entrada invalida.\n");
                    continue;
                }

                switch (opcionCrud)
                {
                case 1:
                    readAllInventario(conn, sesion);
                    break;

                case 2:
                    updateInventario(conn, sesion);
                    break;

                case 3:
                    printf("Volviendo al menu principal...\n");
                    break;

                default:
                    printf("Opcion invalida.\n");
                    break;
                }

            } while (opcionCrud != 3);

            break;
        }

        case 5:
        {
            int opcionCrud = -1;

            do
            {
                printf("\n=== Administrar fichas medicas ===\n");
                printf("1. Crear ficha medica\n");
                printf("2. Actualizar ficha medica\n");
                printf("3. Mostrar todas las fichas medicas\n");
                printf("4. Mostrar ficha medica por ID\n");
                printf("5. Borrar ficha medica por ID\n");
                printf("6. Volver al menu principal\n");
                printf("Selecciona una opcion: ");

                opcionCrud = getInteger();

                if (opcionCrud == -1)
                {
                    printf("Entrada invalida.\n");
                    continue;
                }

                switch (opcionCrud)
                {
                case 1:
                    createFichaMedica(conn, sesion);
                    break;

                case 2:
                    updateFichaMedica(conn, sesion);
                    break;

                case 3:
                    readAllFichaMedica(conn, sesion);
                    break;

                case 4:
                    readFichaMedica(conn, sesion);
                    break;

                case 5:
                    deleteFichaMedica(conn, sesion);
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

        case 6:
        {
            int opcionCrud = -1;

            do
            {
                printf("\n=== Administrar recetas ===\n");
                printf("1. Crear receta\n");
                printf("2. Actualizar receta\n");
                printf("3. Mostrar todas las recetas\n");
                printf("4. Mostrar recetas por paciente\n");
                printf("5. Volver al menu principal\n");
                printf("Selecciona una opcion: ");

                opcionCrud = getInteger();

                if (opcionCrud == -1)
                {
                    printf("Entrada invalida.\n");
                    continue;
                }

                switch (opcionCrud)
                {
                case 1:
                    createReceta(conn, sesion);
                    break;

                case 2:
                    updateReceta(conn, sesion);
                    break;

                case 3:
                    readAllReceta(conn, sesion);
                    break;

                case 4:
                    readReceta(conn, sesion);
                    break;

                case 5:
                    printf("Volviendo al menu principal...\n");
                    break;

                default:
                    printf("Opcion invalida.\n");
                    break;
                }

            } while (opcionCrud != 5);

            break;
        }

        case 7:
            printf("Saliendo de VetFlux...\n");
            break;

        default:
            printf("Opcion invalida.\n");
            break;
        }

    } while (opcion != 7);
}
