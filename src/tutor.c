#include <stdio.h>
#include <string.h>
#include "tutor.h"
#include "utils.h"

// Create tutor
void create_Tutor(PGconn *conn){
    Tutor tutor;

    printf("RUT: ");
    input_string(tutor.rut, sizeof(tutor.rut));
       
}
