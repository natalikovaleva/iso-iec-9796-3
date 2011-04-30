#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "signd.h"
#include "signd-crypto.h"
#include "signd-server.h"

static int server_loop = 1;

static void SIGALRM_handler(int signum __attribute__((unused)))
{
    fprintf(stderr,"SIGALRM to %d\n", getpid());
    exit(1);
}

static void SIGCHLD_handler(int signum __attribute__((unused)))
{
    int status;
    pid_t pid = wait(&status);

    if (pid != -1)
        fprintf(stderr,"[%d]: died with exit = %d\n", pid, status);
}

static void SIGINT_handler(int signum __attribute__((unused)))
{
    fprintf(stderr, "Get SIGINTR signal. Shutting down\n");
    server_loop = 0;
}

int  signd_server_loop(struct signd_config * signd_config,
                       struct signd_crypto * signd_crypto)
{
    /* Configure signal handlers */

    struct sigaction handler = { .sa_flags = SA_RESTART };

    handler.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &handler, NULL);
    handler.sa_handler = SIGCHLD_handler;
    sigaction(SIGCHLD, &handler, NULL);

    handler.sa_flags   = 0x0;
    handler.sa_handler = SIGINT_handler;
    sigaction(SIGINT,  &handler, NULL);

    /* Configure sockets  */

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sock == -1)
        return -1;

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(signd_config->server_port);
    server_addr.sin_addr.s_addr = inet_addr(signd_config->server_ip);

    if ((bind(server_sock,
              (struct sockaddr *) &server_addr,
              sizeof(server_addr)) != 0) ||
        (listen(server_sock, 0) != 0))
    {
        close(server_sock);
        return -1;
    }

    /* Main loop */

    while (server_loop)
    {
        struct sockaddr_in client_addr;
        int client_addr_size = sizeof(client_addr);

        int client_sock = accept(server_sock,
                                 (struct sockaddr *) &client_addr,
                                 &client_addr_size);

        if (client_sock == -1)
            if (errno == EINTR)
                continue;
            else
                return -1;

        pid_t sign_process_pid = fork();

        if (sign_process_pid == 0)
        {
            close(server_sock);

            /* Set alarm */

            handler.sa_handler = SIGALRM_handler;
            sigaction(SIGALRM, &handler, NULL);
            alarm(signd_config->thread_timeout);

            /* Ok. Wait for data to sign. */

            size_t message_size = 65536;
            char * message = alloca(message_size);

            int rval = recv_message(client_sock, message, &message_size);

            fprintf(stderr, "[%d]: Get message from %s:%d: ",
                    getpid(),
                    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            if (rval != 0)
            {
                fprintf(stderr, "RECV ERROR <%s>\n",
                        rval > 0 ? "protocol error" : strerror(errno));
                shutdown(client_sock, SHUT_RDWR);
                close(client_sock);
                return 2;
            }

            struct SIGN * s = sign(message, message_size, signd_crypto);

            if (s == NULL)
            {
                fprintf(stderr, "couldn't create signature\n");
                shutdown(client_sock, SHUT_RDWR);
                close(client_sock);
                return 3;
            }
            else
            {
                fprintf(stderr, "Sign: [%s;%s]; ", s->R, s->S);
            }

            rval = send_sign(client_sock, s);

            if (rval != 0)
            {
                fprintf(stderr, "SEND ERROR <%s>\n",
                        rval > 0 ? "protocol error" : strerror(errno));
            }

            fprintf(stderr, "Sign sended\n");

            free_sign(s);
            shutdown(client_sock, SHUT_RDWR);
            close(client_sock);

            return rval == 0 ? 0 : 4;
        }
        else
        {
            close(client_sock);

            fprintf(stderr, "Create process %d for %s:%d\n",
                    sign_process_pid,
                    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }

    }

    shutdown(server_sock, SHUT_RDWR);
    close(server_sock);
    return 0;
}
