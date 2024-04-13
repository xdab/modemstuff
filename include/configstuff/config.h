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

int cs_config_read(cs_config_t *config, const char *config_file);

const char *cs_config_get(cs_config_t *config, const char *key);

const char *cs_config_get_or_exit(cs_config_t *config, const char *key);

#endif // _CONFIGSTUFF_CONFIG_H