#ifndef API_H
#define API_H

#include "mongoose.h"
#include <libpq-fe.h>

typedef struct api_context {
    PGconn *conn;
} ApiContext;

void apiEventHandler(struct mg_connection *c, int ev, void *ev_data);

#endif
