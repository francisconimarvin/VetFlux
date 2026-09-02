#include <stdio.h>
#include "utils.h"
#include <string.h>


void input_string(char *buffer, size_t size) {
    fgets(buffer, size, stdin);
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
};

