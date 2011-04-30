#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <getopt.h>
#include <malloc.h>
#include <alloca.h>

#include "signd.h"
#include "signd-crypto.h"
#include "signd-server.h"

static struct signd_config * read_config(const char * path)
{
    struct signd_config * config =
        (struct signd_config *) malloc(sizeof(*config));

    bzero(config, sizeof(*config));

    config_init (& config->cnf);
    if (config_read_file(&config->cnf, path) == CONFIG_FALSE)
    {
        return NULL;
    }


#if CONFIG_TRUE != 1
#error "CONFIG_TRUE != 1"
#endif

    int rval = CONFIG_TRUE;
    int port;

    rval &= config_lookup_string(&config->cnf, "Server.Address", &config->server_ip);
    rval &= config_lookup_int   (&config->cnf, "Server.Port",    &port);
            config->server_port = ((short) port);
    rval &= config_lookup_int   (&config->cnf, "Server.Timeout", &config->thread_timeout);
    rval &= config_lookup_string(&config->cnf, "Crypto.Config",  &config->crypto_path);

    if (rval != CONFIG_TRUE)
    {
        fprintf(stderr, "Couldn't read server config from %s\n", path);
        return NULL;
    }

    return config;

}

static void close_config(struct signd_config * config)
{
    config_destroy(&config->cnf);
    free(config);
}


int main(int argc, char *argv[])
{

    start_prng();

    struct signd_config * config = read_config("./signd.conf");

    if (config == NULL)
    {
        fprintf(stderr, "Couldn't read configuration file\n");
        return 1;
    }

    struct signd_crypto * crypto = init_crypto(config->crypto_path);

    if (crypto == NULL)
    {
        fprintf(stderr, "Couldn't create crypto context\n");
        close_config(config);
        return 1;
    }

    int rval = signd_server_loop(config, crypto);

    destroy_crypto(crypto);
    close_config(config);
    stop_prng();

    return rval;
}
