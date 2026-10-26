#include <stdlib.h>
#include <stdio.h>
#include "db.h"
#include "api.h"
#include "mongoose.h"

int main(void) {
    const char *port = getenv("PORT");
    char address[64];
    snprintf(address, sizeof(address), "http://0.0.0.0:%s", port ? port : "8000");

    ApiContext ctx = {0};
    ctx.conn = dbConnect();
    if (ctx.conn == NULL) {
        fprintf(stderr, "No se pudo conectar a PostgreSQL\n");
        return EXIT_FAILURE;
    }

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    if (mg_http_listen(&mgr, address, apiEventHandler, &ctx) == NULL) {
        fprintf(stderr, "No se pudo iniciar el servidor HTTP en %s\n", address);
        mg_mgr_free(&mgr);
        PQfinish(ctx.conn);
        return EXIT_FAILURE;
    }
    printf("VetFlux escuchando en %s\n", address);
    for (;;) mg_mgr_poll(&mgr, 1000);

    mg_mgr_free(&mgr);
    PQfinish(ctx.conn);
    return EXIT_SUCCESS;
}
