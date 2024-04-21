#ifndef _TEST_DEMOD_CONFIG_H
#define _TEST_DEMOD_CONFIG_H

#include <configstuff/config.h>

typedef struct test_demod_config
{
    char file[CONFIG_MAX_VALUE_LENGTH];
    int sample_rate;
    float mark_freq;
    float space_freq;
    float baud_rate;
} test_demod_config_t;

/**
 * Read configuration file.
 *
 * @param config Configuration structure.
 * @param config_file Configuration file.
 *
 * @return 0 if success, otherwise if error.
 */
int test_demod_config_read(
    test_demod_config_t *config,
    const char *config_file);

#endif // _TEST_DEMOD_CONFIG_H