#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define PORT "8080"
#define BACKLOG 50

// Read the manual and the Beej's Guide
void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
int create_socket(const char *port);
void run_server(int sockfd);

#endif
