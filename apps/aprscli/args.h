#ifndef _APRSCLI_ARGS_H
#define _APRSCLI_ARGS_H

#include <stdbool.h>

typedef enum aprscli_packet_type
{
    APRSCLI_PACKET_TYPE_POSITION_REPORT = 'P',
    APRSCLI_PACKET_TYPE_STATUS = 'S'
} aprscli_packet_type_t;

typedef struct aprscli_args
{
    // Positional
    char *host_port;

    // Basic
    char *source;
    char *destination;
    aprscli_packet_type_t type;

    // Common
    char symbol[2];
    char *text;

    // Position report
    double latitude;
    double longitude;
    bool compressed;

    // Extra
    bool confirm_before_sending;

} aprscli_args_t;

int aprscli_parse_args(int argc, char **argv, aprscli_args_t *args);

#endif
