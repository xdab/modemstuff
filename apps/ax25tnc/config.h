#ifndef _AX25TNC_CONFIG_H
#define _AX25TNC_CONFIG_H

#include <configstuff/config.h>

typedef struct ax25tnc_config
{
    char host[CONFIG_MAX_VALUE_LENGTH];
    int port;
    int kiss_port;
    int sample_rate;
    float mark_freq;
    float space_freq;
    float baud_rate;
} ax25tnc_config_t;

/**
 * Read configuration file.
 *
 * @param config Configuration structure.
 * @param config_file Configuration file.
 *
 * @return 0 if success, otherwise if error.
 */
int ax25tnc_config_read(
    ax25tnc_config_t *config,
    const char *config_file);

#endif // _AX25TNC_CONFIG_H