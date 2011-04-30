#pragma once

#include <sys/types.h>
#include "signd-crypto.h"

extern int recv_sign(int fd, struct SIGN ** new_sign);
extern int send_sign(int fd, const struct SIGN * sign);
extern int recv_message(int fd, char * message, size_t * message_size);
extern int send_message(int fd, const char * message, size_t message_size);

