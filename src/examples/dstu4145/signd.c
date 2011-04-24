#include "dstu4145.hpp"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <signal.h>

#include <libconfig.h>

static FILE * prng_source = NULL;

struct EC_Config
{
    config_t cnf;

    const char * P;
    const char * A;
    const char * B;
    const char * GX;
    const char * GY;
    const char * N;
    const char * Q;

    enum DSTU4145_HASHES Hash;
};


void prng (unsigned char * buffer, int count)
{
    fread(buffer, count, 1, prng_source);
}

struct  EC_Config * read_config(const char * path)
{
    struct EC_Config * newConf =
        (struct EC_Config *) malloc(sizeof(struct EC_Config));

    bzero(newConf, sizeof(*newConf));

    config_init (& newConf->cnf);
    if (config_read_file(&newConf->cnf, path) == CONFIG_FALSE)
    {
        return NULL;
    }


#if CONFIG_TRUE != 1
#error "CONFIG_TRUE != 1"
#endif

    int rval = CONFIG_TRUE;

    rval &= config_lookup_string(&newConf->cnf, "EC.P", &newConf->P);
    rval &= config_lookup_string(&newConf->cnf, "EC.A", &newConf->A);
    rval &= config_lookup_string(&newConf->cnf, "EC.B", &newConf->B);
    rval &= config_lookup_string(&newConf->cnf, "EC.N", &newConf->N);
    rval &= config_lookup_string(&newConf->cnf, "EC.G.X", &newConf->GX);
    rval &= config_lookup_string(&newConf->cnf, "EC.G.Y", &newConf->GY);

    if (rval != CONFIG_TRUE)
    {
        fprintf(stderr, "Couldn't read EC data from %s\n", path);
        return NULL;
    }

    return newConf;
}

void close_config(struct EC_Config * config)
{
    config_destroy(&config->cnf);
}



int main(int argc, char *argv[])
{
    prng_source = popen("dd if=/dev/urandom bs=4","r");

    struct EC_Config * config = read_config("./ec.conf");

    if (config == NULL)
    {
        fprintf(stderr, "Couldn't read EC from config\n");
        return 1;
    }


    void * EC = dstu4145_createEC_ZZ_p(config->P,
                                       config->A,
                                       config->B,
                                       config->GX,
                                       config->GY,
                                       config->N);

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

    struct SIGN * sign;

    do
    {
        sign = dstu4145_create_sign(Message, sizeof(Message), dstu4145);
        printf("R: %s\nS: %s\nM: %s/%d\n", sign->R, sign->S, sign->M, sign->M_size);
        printf("V: %s\n", dstu4145_verify_sign(sign, dstu4145));
        dstu4145_free_sign(sign);
    } while (1);


  lbExit:

    dstu4145_free_context(dstu4145);
    dstu4145_freeEC_ZZ_p(EC);
    return 0;
}
