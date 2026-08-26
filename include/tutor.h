#ifndef TUTOR_H
#define TUTOR_H

#include "libpq-fe.h"

void menu_Tutor(PGconn *conn);

// CRUD
void create_Tutor(PGconn *conn);
void read_Tutor(PGconn *conn);
void update_Tutor(PGconn *conn);
void delete_Tutor(PGconn *conn);

#endif
