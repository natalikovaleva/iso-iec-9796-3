#pragma once

#include "dstu4145.hpp"

struct SIGND_Context
{
    void * EC;
    void * dstu4145;
};

struct SIGND_Context * init_crypto(const char * KeyData);
void destroy_crypto(struct SIGND_Context * ctx);

struct SIGN * sign(const char * Message, int message_size,
                   struct SIGND_Context * ctx);
void free_sign(struct SIGN * s);

const char * verify(struct SIGN * s,
                    struct SIGND_Context * ctx);



int start_prng();
int stop_prng();
