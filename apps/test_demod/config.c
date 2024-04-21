#include "config.h"

#include <stdio.h>
#include <string.h>

#define FILE_KEY "FILE"
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

int test_demod_config_read(test_demod_config_t *config, const char *config_file)
{
    cs_config_t cs_config;
    char *file;

    if (cs_config_read(&cs_config, config_file))
        _read_error();

    file = NULL;
    if (cs_config_get_str(&cs_config, FILE_KEY, &file))
        _parameter_not_supplied_error(FILE_KEY);
    strncpy(config->file, file, CONFIG_MAX_VALUE_LENGTH);

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