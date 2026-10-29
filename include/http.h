#ifndef HTTP_H
#define HTTP_H

#define BUF_SIZE 4096

#include <sys/types.h>

typedef struct httpRequest {
  char method[8];
  char path[256];
  char version[16];
} HttpRequest;

int handle_http_request(int client_fd);

#endif
