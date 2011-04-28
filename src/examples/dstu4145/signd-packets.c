#include <string.h>
#include <malloc.h>

#include "signd-packets.h"

/* GENERIC PROTOCOL:
 * 1. [ 1b ] TYPE
 * 2. [ .. ] Packet
 * MESSAGE PROTOCOL:
 * 1. [ 1b ] TYPE
 * 2. [ 2b ] Message size
 * 3. [ .. ] Message data
 * SIGN PROTOCOL:
 * 1. [ 1b ] TYPE
 * 2. [ 1b ] R-Size
 * 3. [ 1b ] S-Size
 * 4. [ 2b ] Message size
 * 5. [ .. ] R
 * 6. [ .. ] S
 * 7. [ .. ] Message
 * */

enum PACKAGE_TYPE
{
    PACKAGE_MESSAGE = 0,
    PACKAGE_SIGN,
};

int send_message(int fd, const char * message, size_t message_size)
{
    if (message_size != ((size_t)((short) message_size)))
        return 1;

    const char type = PACKAGE_MESSAGE;
    const short size = htons((short) message_size);

    if ((send(fd, &type, sizeof(type), 0) != sizeof(type)) ||
        (send(fd, &size, sizeof(size), 0) != sizeof(size)))
        return -1;

    int send_pos  = 0;
    int send_size = message_size;

    do {
        int sended = send(fd, message + send_pos, message_size, 0);
        if (sended == -1)
        {
            return -1;
        }
        send_pos  += sended;
        send_size -= sended;
    } while(send_size);

    return 0;
}

int recv_message(int fd, char * message, size_t * message_size)
{
    char  type = 0;
    short size = 0;

    if (recv(fd, &type, sizeof(type), 0) != sizeof(type))
    {
        fprintf(stderr, "DBG: recv type\n");
        return -1;
    }


    if (type != PACKAGE_MESSAGE)
        return 1;

    if (recv(fd, &size, sizeof(size), 0) != sizeof(size))
    {
        fprintf(stderr, "DBG: recv size\n");
        return -1;
    }

    size = ntohs(size);

    if (size > *message_size)
        return 1;

    int recv_pos  = 0;
    int recv_size = size;

    do {
        int recved = recv(fd, message + recv_pos, recv_size, 0);
        if (recved == -1)
        {
            fprintf(stderr, "DBG: recv message: %d\n", size);
            *message_size = 0;
            return -1;
        }
        recv_pos  += recved;
        recv_size -= recved;
    } while(recv_size);

    *message_size = size;

    return 0;
}


int send_sign(int fd, const struct SIGN * s)
{
    if (s == NULL)
        return 1;

    const size_t R_size_ = strlen(s->R);
    const size_t S_size_ = strlen(s->S);

    if ((R_size_ > 255) ||
        (S_size_ > 255) ||
        (s->M_size > 65535))
        return 1;

    const char  type = PACKAGE_SIGN;
    const char  R_size = (char)R_size_;
    const char  S_size = (char)S_size_;
    const short M_size = htons((short) s-> M_size);

    if ((send(fd, &type,   sizeof(type),  0) != sizeof(type))   ||
        (send(fd, &R_size, sizeof(R_size),0) != sizeof(R_size)) ||
        (send(fd, &S_size, sizeof(S_size),0) != sizeof(S_size)) ||
        (send(fd, &M_size, sizeof(M_size),0) != sizeof(M_size)) ||
        (send(fd, s->R,    R_size,        0) != R_size)         ||
        (send(fd, s->S,    S_size,        0) != S_size)         ||
        (send(fd, s->M,    s->M_size,     0) != s->M_size))
        return -1;

    return 0;
}

int recv_sign(int fd, struct SIGN ** new_sign)
{
    if (new_sign == NULL)
        return 1;

    char type;

    if (recv(fd, &type, sizeof(type)) != sizeof(type))
        return -1;

    if (type != PACKAGE_SIGN)
        return 1;

    char  R_size;
    char  S_size;
    short M_size;

    if ((recv(fd, &R_size, sizeof(R_size),0) != sizeof(R_size)) ||
        (recv(fd, &M_size, sizeof(S_size),0) != sizeof(S_size)) ||
        (recv(fd, &M_size, sizeof(M_size),0) != sizeof(M_size)))
        return -1;

    M_size = ntohs(M_size);

    *new_sign = (struct SIGN *) (malloc(sizeof(struct SIGN)));
    if (*new_sign == NULL)
        return 1;

    (*new_sign)->R = (char *) malloc (R_size + 1);
    (*new_sign)->S = (char *) malloc (S_size + 1);
    (*new_sign)->M = (char *) malloc (M_size);

    if ((((*new_sign)->R == NULL) ||
         ((*new_sign)->S == NULL) ||
         ((*new_sign)->M == NULL))
        ||
        ((recv(fd, (*new_sign)->R, R_size, 0) != R_size) ||
         (recv(fd, (*new_sign)->S, S_size, 0) != S_size) ||
         (recv(fd, (*new_sign)->M, M_size, 0) != M_size)))
    {
        free((*new_sign)->R);
        free((*new_sign)->S);
        free((*new_sign)->M);
        free(*new_sign);
        *new_sign = NULL;
        return -1;
    }

    (*new_sign)->R[R_size] = '\0';
    (*new_sign)->S[S_size] = '\0';
    (*new_sign)->M_size = M_size;

    return 0;
}
