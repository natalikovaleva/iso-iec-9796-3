#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <getopt.h>
#include <malloc.h>
#include <alloca.h>
#include <errno.h>

#include "signd-crypto.h"
#include "signd-packets.h"

int main(int argc, char *argv[])
{
    if (argc !=4)
    {
        fprintf(stdout, "Usage: %s host port message.file\n", argv[0]);
        return 1;
    }

    if (access(argv[3],R_OK) == -1)
    {
        perror("No access: ");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        perror("Couldn't create socket: ");
        return 1;
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0)
    {
        perror("Couldn't connect to server: ");
        return 1;
    }

    int msg_fd = open(argv[3], O_RDONLY);
    if (msg_fd == -1)
    {
        perror("Couldn't open message file: ");
        return 1;
    }

    struct stat stat;
    fstat(msg_fd, &stat);

    if (stat.st_size > 65535)
    {
        printf("Message file is too big\n");
        return 1;
    }

    char * buffer = alloca(stat.st_size);

    if (read(msg_fd, buffer, stat.st_size) != stat.st_size)
    {
        perror("Couldn't read message: ");
        return 1;
    }

    if( send_message(sock, buffer, stat.st_size) != 0)
    {
        printf("FD: %d\n", sock);

        perror("Couldn't send message: ");
        return 1;
    }

    struct SIGN * sign = NULL;

    if ( recv_sign(sock, &sign) != 0)
    {
        perror("Couldn't recv sign: ");
        return 1;
    }

    struct signd_crypto * ctx = init_crypto("client.pkey");

    if (ctx == NULL)
    {
        printf("Couldn't create crypto context\n");
        return 1;
    }

    const char * msg = verify(sign, ctx);

    if (msg == NULL)
    {
        printf("Couldn't verify signature\n");
        return 1;
    }

    printf("Signature valid:\nR: %s\nS: %s\n", sign->R, sign->S);

    free_sign(sign);
    destroy_crypto(ctx);

    return 0;
}
