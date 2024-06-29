#ifndef _APRSCLI_ARGS_H
#define _APRSCLI_ARGS_H

#include <stdbool.h>

typedef enum aprscli_packet_type
{
    APRSCLI_PACKET_TYPE_POSITION_REPORT = 'P',
    APRSCLI_PACKET_TYPE_STATUS = 'S',
    APRSCLI_PACKET_TYPE_OBJECT = 'O',
    APRSCLI_PACKET_TYPE_ITEM = 'I',
    APRSCLI_PACKET_TYPE_MESSAGE = 'M',
} aprscli_packet_type_t;

typedef struct aprscli_args
{
    // Basic
    char *source;
    char *destination;
    aprscli_packet_type_t type;

    // Common
    char *text;

    // Position
    double latitude;
    double longitude;
    bool compressed;
    char symbol[2];

    // Time
    char *time;
    bool time_local;
    bool time_hms;

    // Object/Item
    char *name;
    bool killed;

    // Position report
    bool messaging;

    // Message
    char *recipient;
    int sequence_number;
    bool ack;
    bool rej;

    // KISS
    bool kiss;
    int kiss_port;

} aprscli_args_t;

int aprscli_parse_args(int argc, char **argv, aprscli_args_t *args);

#endif
