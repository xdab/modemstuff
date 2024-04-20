#include <configstuff/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CONFIG_MAX_LINE_LENGTH (CONFIG_MAX_KEY_LENGTH + CONFIG_MAX_VALUE_LENGTH + 1)

int cs_config_read(cs_config_t *config, const char *config_file)
{
    char line_buf[CONFIG_MAX_LINE_LENGTH];
    FILE *file;
    char *key, *value, *line, *back;

    config->entries_count = 0;

    file = fopen(config_file, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: fopen() failed\n");
        return -1;
    }

    while ((config->entries_count < CONFIG_MAX_ENTRIES) && (fgets(line_buf, CONFIG_MAX_LINE_LENGTH, file) != NULL))
    {
        // Remove leading and trailing whitespace
        line = line_buf;
        while (isspace(*line))
            line++;
        back = line + strlen(line);
        while (isspace(*--back))
            ;
        *(back + 1) = 0;

        // Skip empty lines
        if (strlen(line) == 0)
            continue;

        // Skip comments
        if (line[0] == '#' || line[0] == ';')
            continue;

        // Skip non-printable characters
        if (line[0] < 32 || line[0] > 126)
            continue;

        key = strtok(line, " ");
        if (key == NULL)
            continue;

        strcpy(config->entries[config->entries_count].key, key);

        value = strtok(NULL, " ");
        if (value == NULL)
            continue;

        strcpy(config->entries[config->entries_count].value, value);
        config->entries_count++;
    }

    fclose(file);

    return 0;
}

int cs_config_get_str(cs_config_t *config, const char *key, char **out_value)
{
    int i;

    for (i = 0; i < config->entries_count; i++)
        if (strcmp(config->entries[i].key, key) == 0)
        {
            if (out_value != NULL)
                *out_value = config->entries[i].value;
            return 0;
        }

    return 1;
}

int cs_config_get_int(cs_config_t *config, const char *key, int *out_value)
{
    char *value;
    int key_not_found;

    key_not_found = cs_config_get_str(config, key, &value);
    if (key_not_found)
        return key_not_found;

    if (out_value != NULL)
        *out_value = atoi(value);

    return 0;
}

int cs_config_get_float(cs_config_t *config, const char *key, float *out_value)
{
    char *value;
    int key_not_found;

    key_not_found = cs_config_get_str(config, key, &value);
    if (key_not_found)
        return key_not_found;

    if (out_value != NULL)
        *out_value = atof(value);

    return 0;
}