#pragma once

#include <libconfig.h>

struct signd_config
{
    int   thread_timeout;
    short server_port;
    const char * server_ip;
    const char * crypto_path;

    config_t cnf;
};

