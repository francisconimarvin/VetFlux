#ifndef DB_H
#define DB_H

#include "libpq-fe.h"

PGconn *dbConnect(void);

#endif //DB_H
