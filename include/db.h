#ifndef DB_H
#define DB_H

#include "libpq-fe.h"

PGconn *db_connect(void);

#endif //DB_H
