#include <stdio.h>
#include "utils.h"
#include <string.h>


void input_string(char *buffer, size_t size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
};


