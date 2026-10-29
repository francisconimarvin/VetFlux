#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <server.h>
#include <http.h>

int handle_http_request(int client_fd)
{
    char buffer[BUF_SIZE];
    size_t received;
    ssize_t bytes;
    char *end_headers;
    HttpRequest request;

    received = 0;

    while (1)
    {
        if (received >= sizeof(buffer) - 1)
            return -1;

        bytes = recv(client_fd,
                buffer + received,
                sizeof(buffer) - received - 1,
                0);

        if (bytes <= 0)
            return -1;

        received += (size_t) bytes;
        buffer[received] = '\0';

        end_headers = strstr(buffer, "\r\n\r\n");

        if (end_headers != NULL)
            break;
    }

    if (sscanf(buffer, "%7s %255s %15s",
                request.method,
                request.path,
                request.version) != 3)
        return -1;

    printf("Método: %s\n", request.method);
    printf("Ruta: %s\n", request.path);
    printf("Versión: %s\n", request.version);

    return 0; 
}
