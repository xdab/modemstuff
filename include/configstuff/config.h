#ifndef _CONFIGSTUFF_CONFIG_H
#define _CONFIGSTUFF_CONFIG_H

#define CONFIG_MAX_ENTRIES (32)
#define CONFIG_MAX_KEY_LENGTH (31 + 1)    // +1 for null terminator
#define CONFIG_MAX_VALUE_LENGTH (127 + 1) // +1 for null terminator

typedef struct cs_config_entry
{
    char key[CONFIG_MAX_KEY_LENGTH];
    char value[CONFIG_MAX_VALUE_LENGTH];
} cs_config_entry_t;

typedef struct cs_config
{
    cs_config_entry_t entries[CONFIG_MAX_ENTRIES];
    int entries_count;
} cs_config_t;

/**
 * Read configuration file.
 *
 * @param config Configuration structure.
 * @param config_file Configuration file.
 *
 * @return 0 if success, otherwise if error.
 */
int cs_config_read(
    cs_config_t *config,
    const char *config_file);

/**
 * Get configuration value as string.
 *
 * @param config Configuration structure.
 * @param key Configuration key.
 * @param out_value Pointer to char array to store configuration value in.
 *
 * @return 0 if success, otherwise if error.
 */
int cs_config_get_str(
    cs_config_t *config,
    const char *key,
    char **out_value);

/**
 * Get configuration value as integer.
 *
 * @param config Configuration structure.
 * @param key Configuration key.
 * @param out_value Pointer to integer to store configuration value in.
 *
 * @return 0 if success, otherwise if error.
 */
int cs_config_get_int(
    cs_config_t *config,
    const char *key,
    int *out_value);

/**
 * Get configuration value as float.
 *
 * @param config Configuration structure.
 * @param key Configuration key.
 * @param out_value Pointer to float to store configuration value in.
 *
 * @return 0 if success, otherwise if error.
 */
int cs_config_get_float(
    cs_config_t *config,
    const char *key,
    float *out_value);

#endif // _CONFIGSTUFF_CONFIG_H