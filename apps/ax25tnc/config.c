#include "config.h"

#include <stdio.h>
#include <string.h>

#define HOST_KEY "HOST"
#define PORT_KEY "PORT"
#define KISS_PORT_KEY "KISSPORT"
#define SAMPLE_RATE_KEY "RATE"
#define MARK_FREQ_KEY "MARK"
#define SPACE_FREQ_KEY "SPACE"
#define BAUD_RATE_KEY "BAUD"

#define _read_error()                                                  \
    {                                                                  \
        fprintf(stderr, "Error: Unable to read configuration file\n"); \
        return 1;                                                      \
    }

#define _parameter_not_supplied_error(p)                                        \
    {                                                                           \
        fprintf(stderr, "Error: %s configuration parameter not supplied\n", p); \
        return 2;                                                               \
    }

int ax25tnc_config_read(ax25tnc_config_t *config, const char *config_file)
{
    cs_config_t cs_config;
    char *host;

    if (cs_config_read(&cs_config, config_file))
        _read_error();

    host = NULL;
    if (cs_config_get_str(&cs_config, HOST_KEY, &host))
        _parameter_not_supplied_error(HOST_KEY);
    strncpy(config->host, host, CONFIG_MAX_VALUE_LENGTH);

    if (cs_config_get_int(&cs_config, PORT_KEY, &config->port))
        _parameter_not_supplied_error(PORT_KEY);

    if (cs_config_get_int(&cs_config, KISS_PORT_KEY, &config->kiss_port))
        _parameter_not_supplied_error(KISS_PORT_KEY);

    if (cs_config_get_int(&cs_config, SAMPLE_RATE_KEY, &config->sample_rate))
        _parameter_not_supplied_error(SAMPLE_RATE_KEY);

    if (cs_config_get_float(&cs_config, MARK_FREQ_KEY, &config->mark_freq))
        _parameter_not_supplied_error(MARK_FREQ_KEY);

    if (cs_config_get_float(&cs_config, SPACE_FREQ_KEY, &config->space_freq))
        _parameter_not_supplied_error(SPACE_FREQ_KEY);

    if (cs_config_get_float(&cs_config, BAUD_RATE_KEY, &config->baud_rate))
        _parameter_not_supplied_error(BAUD_RATE_KEY);

    return 0;
}