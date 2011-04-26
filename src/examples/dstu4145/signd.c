#include "dstu4145.hpp"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <alloca.h>
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

struct SIGND_Context
{
    void * EC;
    void * dstu4145;
};

void prng (unsigned char * buffer, int count)
{
    fread(buffer, count, 1, prng_source);
}

const char * pkey (unsigned char * buffer, int count)
{
    memset(buffer, 0x0, count);

    FILE * devkey = fopen("/dev/keysrv", "r");
    if (devkey == NULL)
        return NULL;

    unsigned char * rawbuf = alloca(count/2);

    if (fread(rawbuf, count/2, 1, devkey) != 1)
    {
        memset(buffer, 0x0, count);
        fprintf(stderr, "Couldn't load private key\n");
        fclose(devkey);

        return NULL;
    }

    fclose(devkey);

    int i;

    for (i = 0; i<count/2; i++)
        sprintf(buffer + (i*2), "%02x", *(rawbuf + i));

    return buffer;
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
    rval &= config_lookup_int(&newConf->cnf, "Hash", (int *) &newConf->Hash);

    if (rval != CONFIG_TRUE)
    {
        fprintf(stderr, "Couldn't read EC data from %s\n", path);
        return NULL;
    }

    rval &= config_lookup_string(&newConf->cnf, "PublicKey", &newConf->Q);

    return newConf;
}

void close_config(struct EC_Config * config)
{
    config_destroy(&config->cnf);
    free(config);
}



struct SIGND_Context * init_crypto(const char * KeyData)
{

    struct SIGND_Context * ctx =
        (struct SIGND_Context *) malloc(sizeof(*ctx));

    if (ctx == NULL)
        return NULL;

    bzero(ctx, sizeof(*ctx));

    struct EC_Config * config = read_config(KeyData);

    if (config == NULL)
    {
        fprintf(stderr, "Couldn't read EC from config\n");
        free(ctx);
        return NULL;
    }


    void * EC = dstu4145_createEC_ZZ_p(config->P,
                                       config->A,
                                       config->B,
                                       config->GX,
                                       config->GY,
                                       config->N);

    if (EC == NULL)
    {
        fprintf(stderr, "Couldn't create EC\n");
        free(ctx);
        close_config(config);
        return NULL;
    }


    char * key = alloca(strlen(config->N) + 1);

    if (pkey(key, strlen(config->N)) == NULL)
    {
        fprintf(stderr, "Couldn't load private key\n");
        dstu4145_freeEC_ZZ_p(EC);
        free(ctx);
        close_config(config);
        return NULL;
    }

    void * dstu4145 = dstu4145_create_context(EC, config->Hash, prng);

    if (dstu4145 == NULL)
    {
        fprintf(stderr, "Couldn't create DSTU 4145 context\n");
        dstu4145_freeEC_ZZ_p(EC);
        free(ctx);
        close_config(config);
        return NULL;
    }

    int rval = 0;

    rval |= dstu4145_set_private_key(key, dstu4145);

    if (config->Q != NULL)
    {
        rval |= dstu4145_set_public_key(config->Q, dstu4145);
    }
    else
    {
        char * public_key = NULL;
        public_key = dstu4145_create_public_key(&public_key, dstu4145);
        if (public_key == NULL)
            rval = 1;
        else
        {
            fprintf(stderr, "Generate public key: %s\n", public_key);
            free(public_key);
        }
    }

    fprintf(stderr, "X: %s\n", key);
    fprintf(stderr, "N: %s\n", config->N);


    if (rval)
    {
        fprintf (stderr, "Couldn't set Public/private key: probably incorrect format\n");
        dstu4145_freeEC_ZZ_p(EC);
        free(ctx);
        close_config(config);
        return NULL;
    }

    dstu4145_make_precoputations(dstu4145);

    ctx->EC = EC;
    ctx->dstu4145 = dstu4145;

    return ctx;
}

void destroy_crypto(struct SIGND_Context * ctx)
{
    dstu4145_free_context(ctx->dstu4145);
    dstu4145_freeEC_ZZ_p(ctx->EC);
    free(ctx);
}

struct SIGN * sign(const char * Message, int message_size,
                   struct SIGND_Context * ctx)
{
    return dstu4145_create_sign(Message, message_size, ctx->dstu4145);
}

void free_sign(struct SIGN * s)
{
    dstu4145_free_sign(s);
}


const char * verify(struct SIGN * s,
                    struct SIGND_Context * ctx)
{
    return dstu4145_verify_sign(s, ctx->dstu4145);
}


int main(int argc, char *argv[])
{
    prng_source = popen("dd if=/dev/urandom bs=4","r");

    struct SIGND_Context * crypto = init_crypto("./ec.conf");

    if (crypto == NULL)
    {
        fprintf(stderr, "Couldn't create crypto context\n");
        return 1;
    }

    const char Message[] = "Hello, world!\n";

    struct SIGN * s = sign(Message, sizeof(Message), crypto);
    printf("R: %s\n", s->R);
    printf("S: %s\n", s->S);
    printf("M: %s\n", s->M);

    printf("V: %s\n", verify(s, crypto));

    free_sign(s);
    destroy_crypto(crypto);
    return 0;
}
