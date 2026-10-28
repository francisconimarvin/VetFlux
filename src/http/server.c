#include <server.h>

// Child handler
void sigchld_handler(int s)
{
    int saved_errno;

    (void)s;

    saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
        /* Reap terminated child processes. */
    }

    errno = saved_errno;
}

// Get the address
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Create, bind and listen on a socket
int create_socket(const char *port)
{
    int sockfd;
    int yes;
    int rv;

    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;

    yes = 1;
    sockfd = -1;
    servinfo = NULL;
    p = NULL;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    rv = getaddrinfo(NULL, port, &hints, &servinfo);

    if (rv != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    // Try each available address
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol);

        if (sockfd == -1)
        {
            perror("server: socket");
            continue;
        }

        // Review the manual for POSIX Programmers for setsockopt()
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                       &yes, sizeof yes) == -1)
        {
            perror("setsockopt");
            close(sockfd);
            sockfd = -1;
            break;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("server: bind");
            close(sockfd);
            sockfd = -1;
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (sockfd == -1)
    {
        fprintf(stderr, "server: failed to create or bind socket\n");
        return -1;
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// Accept and handle incoming connections
void run_server(int sockfd)
{
    int new_fd;
    pid_t pid;

    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;

    char s[INET6_ADDRSTRLEN];

    new_fd = -1;
    sin_size = 0;

    memset(&sa, 0, sizeof sa);

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        return;
    }

    printf("server: waiting for connections...\n");

    while (1)
    {
        sin_size = sizeof their_addr;

        new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
                        &sin_size);

        if (new_fd == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("accept");
            continue;
        }

        if (inet_ntop(their_addr.ss_family,
                      get_in_addr((struct sockaddr *)&their_addr),
                      s, sizeof s) == NULL)
        {
            perror("inet_ntop");
            close(new_fd);
            continue;
        }

        printf("server: got connection from %s\n", s);

        pid = fork();

        if (pid == -1)
        {
            perror("fork");
            close(new_fd);
            continue;
        }

        if (pid == 0)
        {
            close(sockfd);

            if (send(new_fd, "Estás conectado a VetFlux.", 28, 0) == -1)
            {
                perror("send");
            }

            close(new_fd);
            _exit(EXIT_SUCCESS);
        }

        close(new_fd);
    }
}

