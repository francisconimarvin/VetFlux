#include <stdio.h>
#include "utils.h"

void cleanBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
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


