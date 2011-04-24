#include "dstu4145.hpp"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <signal.h>

void prng (unsigned char * buffer, int count)
{
    memset(buffer, rand() % 0xff, count);
}

static int bench = 0;
static int bench_run = 1;

void alarm_SIGNAL(int i __attribute__((unused)))
{
    printf("%d signs / 10s\n", bench);
    bench_run = 0;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
        
    void * EC = dstu4145_createEC_ZZ_p("FFD5D55FA9934410D3EB8BC04648779F13174945",
                                       "710062DCB53DC6E42F8227A4FBAC2240BD3504D4",
                                       "4163E75BB92147D54E09B0F13822B076A0944359",
                                       "3C1E27D71F992260CF3C31C90D80B635E9FD0E68",
                                       "C436EFC0041BBF0947A304A005F8D43A36763031",
                                       "2AA3A38FF1988B58235241EE59A73F4646443245");

    if (EC == NULL)
    {
        printf("Couldn't create EC\n");
        return 1;
    }
    
    
    void * dstu4145 = dstu4145_create_context(EC, SHA512, prng);

    if (dstu4145 == NULL)
    {
        printf("Couldn't create context\n");
        return 1;
    }

    int rval = 0;
    
    rval |= dstu4145_set_private_key("648bcb2e4d5d151656c84774ed016ba292a5a38", dstu4145);
    rval |= dstu4145_set_public_key("9fae226e565e907619a48b598adf534dd63310dd3cf1f2454060f6799bf4e79847276a8f81e964e6",dstu4145);
    rval |= dstu4145_make_precoputations(dstu4145);
        
    if (rval)
    {
        printf ("Couldn't set Public/private key: probably incorrect format\n");
        goto lbExit;
    }

    const char Message[] = "Hello, world!";

    struct sigaction act;
    act.sa_handler = alarm_SIGNAL;
    act.sa_flags = 0x0;
    
    sigaction(SIGALRM, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    alarm(10);

    struct SIGN * sign;
    
    do
    {
        sign = dstu4145_create_sign(Message, sizeof(Message), dstu4145);
        
        if (sign == NULL)
        {
            printf("Couldn't sign message\n");
            goto lbExit;
        }

        bench ++;
    } while(bench_run);
        

    sign = dstu4145_create_sign(Message, sizeof(Message), dstu4145);
    printf("R: %s\nS: %s\nM: %s/%d\n", sign->R, sign->S, sign->M, sign->M_size);
    
    printf("V: %s\n", dstu4145_verify_sign(sign, dstu4145));
    dstu4145_free_sign(sign);
    
  lbExit:
    
    dstu4145_free_context(dstu4145);
    dstu4145_freeEC_ZZ_p(EC);
    return 0;
}
