#include <stdio.h>
#include "utils.h"
#include <libpq-fe.h>
#include <string.h>
void cleanBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

int getInteger(void)
{
    int num;
    int res = scanf("%d", &num);

    if (res != 1)
    {
        cleanBuffer();
        return -1;
    }

    cleanBuffer();
    return num;
}

void printQueryResult(PGresult *res)
{
    int rows = PQntuples(res);
    int columns = PQnfields(res);

    if (rows == 0)
    {
        printf("No se encontraron registros.\n");
        return;
    }

    for (int j = 0; j < columns; j++)
    {
        printf("%-20s", PQfname(res, j));
    }

    printf("\n");

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%-20s", PQgetvalue(res, i, j));
        }

        printf("\n");
    }
}

void getString(char *buffer, size_t size)
{
    if (fgets(buffer, size, stdin) != NULL)
        buffer[strcspn(buffer, "\n")] = '\0';
    else
        buffer[0] = '\0';
}