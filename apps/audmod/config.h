#ifndef _AUDMOD_CONFIG_H
#define _AUDMOD_CONFIG_H

#include <configstuff/config.h>

typedef struct audmod_config
{
    char host[CONFIG_MAX_VALUE_LENGTH];
    int port;
    int kiss_port;
    int sample_rate;
    float mark_freq;
    float space_freq;
    float baud_rate;
} audmod_config_t;

/**
 * Read configuration file.
 *
 * @param config Configuration structure.
 * @param config_file Configuration file.
 *
 * @return 0 if success, otherwise if error.
 */
int audmod_config_read(
    audmod_config_t *config,
    const char *config_file);

#endif // _AUDMOD_CONFIG_H