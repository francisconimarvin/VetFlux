#include <stdio.h>
#include <db.h>
#include <stdlib.h>
#include <server.h>
#include <signal.h>
#include <unistd.h>

int main(void)
{
    int sockfd;
    PGconn *conn;

    sockfd = create_socket(PORT);
    if (sockfd == -1)
    {
        return EXIT_FAILURE;
    }

    conn = dbConnect();
    if (conn == NULL)
    {
        close(sockfd);
        return EXIT_FAILURE;
    }

    run_server(sockfd);

    PQfinish(conn);
    close(sockfd);

    return EXIT_SUCCESS;
}
