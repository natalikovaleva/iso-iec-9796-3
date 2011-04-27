#include <string.h>
#include <stdio.h>
#include <signal.h>

#include "signd-crypto.h"


int main(int argc, char *argv[])
{

    start_prng();

    struct SIGND_Context * crypto = init_crypto("./crypto.conf");

    if (crypto == NULL)
    {
        fprintf(stderr, "Couldn't create crypto context\n");
        return 1;
    }

    const char Message[] = "Hello, world!\n";

    struct SIGN * s = sign(Message, sizeof(Message), crypto);
    if (s)
    {
        printf("R: %s\n", s->R);
        printf("S: %s\n", s->S);
        printf("M: %s\n", s->M);

        printf("V: %s\n", verify(s, crypto));

        free_sign(s);
    }
    else
    {
        fprintf(stderr, "Couldn't create signature. Is private key loaded?\n");
    }


    destroy_crypto(crypto);

    stop_prng();

    return 0;
}
