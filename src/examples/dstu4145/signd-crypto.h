#pragma once

#include "dstu4145.hpp"

struct signd_crypto
{
    void * EC;
    void * dstu4145;
};

extern struct signd_crypto * init_crypto(const char * KeyData);
extern void destroy_crypto(struct signd_crypto * ctx);

extern struct SIGN * sign(const char * Message, int message_size,
                          struct signd_crypto * ctx);
extern void free_sign(struct SIGN * s);

extern const char * verify(struct SIGN * s,
                           struct signd_crypto * ctx);

extern int start_prng();
extern int stop_prng();
extern void prng (unsigned char * buffer, int count);
