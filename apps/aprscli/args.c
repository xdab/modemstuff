#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>

// Basic

#define ARG_SOURCE_KEY 's'
#define ARG_SOURCE "src"
#define ARG_SOURCE_ARG "CALL"
#define ARG_SOURCE_FLAGS (0)
#define ARG_SOURCE_DESC "Source callsign"
#define ARG_SOURCE_DEFAULT "N0CALL"

#define ARG_DESTINATION_KEY 'd'
#define ARG_DESTINATION "dest"
#define ARG_DESTINATION_ARG "CALL"
#define ARG_DESTINATION_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_DESTINATION_DESC "Destination callsign (default: APRS)"
#define ARG_DESTINATION_DEFAULT "APRS"

#define ARG_TYPE_KEY 't'
#define ARG_TYPE "type"
#define ARG_TYPE_ARG "TYPE"
#define ARG_TYPE_FLAGS 0
#define ARG_TYPE_DESC "Type of APRS packet\n\t(P)osition report, (S)tatus, (O)bject, (I)tem, (M)essage"
#define ARG_TYPE_DEFAULT 0

// Common

#define ARG_TEXT_KEY 'x'
#define ARG_TEXT "text"
#define ARG_TEXT_ARG "TEXT"
#define ARG_TEXT_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_TEXT_DESC "Text (common to types: P,S,O,I,M)\n\t(default: No text)"
#define ARG_TEXT_DEFAULT 0

// Position

#define ARG_LATITUDE_KEY 'l'
#define ARG_LATITUDE "lat"
#define ARG_LATITUDE_ARG "LAT"
#define ARG_LATITUDE_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_LATITUDE_DESC "Latitude (common to types: P,O,I)\n\t(default: 0.0)"
#define ARG_LATITUDE_DEFAULT 0.0

#define ARG_LONGITUDE_KEY 'L'
#define ARG_LONGITUDE "lon"
#define ARG_LONGITUDE_ARG "LON"
#define ARG_LONGITUDE_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_LONGITUDE_DESC "Longitude (common to types: P,O,I)\n\t(default: 0.0)"
#define ARG_LONGITUDE_DEFAULT 0.0

#define ARG_SYMBOL_KEY 'S'
#define ARG_SYMBOL "symbol"
#define ARG_SYMBOL_ARG "SYMBOL"
#define ARG_SYMBOL_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_SYMBOL_DESC "Symbol (default: /- house)"
#define ARG_SYMBOL_DEFAULT "/-"

#define ARG_COMPRESSED_KEY 'c'
#define ARG_COMPRESSED "compressed"
#define ARG_COMPRESSED_ARG 0
#define ARG_COMPRESSED_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_COMPRESSED_DESC "Compressed position report (default: false)"
#define ARG_COMPRESSED_DEFAULT 0

// Time

#define ARG_TIME_KEY ('t' + 1000)
#define ARG_TIME "time"
#define ARG_TIME_ARG "TIME"
#define ARG_TIME_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_TIME_DESC "Time in ISO 8601 (YYYY-MM-DDThh:mm:ss) format (default: current time)"
#define ARG_TIME_DEFAULT 0

#define ARG_TIME_LOCAL_KEY ('l' + 1000)
#define ARG_TIME_LOCAL "local"
#define ARG_TIME_LOCAL_ARG 0
#define ARG_TIME_LOCAL_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_TIME_LOCAL_DESC "Use local time for timestamp instead of UTC (default: false)"
#define ARG_TIME_LOCAL_DEFAULT 0

#define ARG_TIME_HMS_KEY ('h' + 1000)
#define ARG_TIME_HM "dhm"
#define ARG_TIME_DHM_ARG 0
#define ARG_TIME_DHM_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_TIME_DHM_DESC "Use DHM format for timestamp instead of HMS (default: false)"
#define ARG_TIME_DHM_DEFAULT 0

// Position report

#define ARG_MESSAGING_KEY ('m' + 1000)
#define ARG_MESSAGING "messaging"
#define ARG_MESSAGING_ARG 0
#define ARG_MESSAGING_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_MESSAGING_DESC "Messaging (default: false)"
#define ARG_MESSAGING_DEFAULT 0

// Object/Item

#define ARG_NAME_KEY 'n'
#define ARG_NAME "name"
#define ARG_NAME_ARG "NAME"
#define ARG_NAME_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_NAME_DESC "Name (common to types: O,I)\n\t(default: NAME)"
#define ARG_NAME_DEFAULT "NAME"

#define ARG_KILLED_KEY ('k' + 1000)
#define ARG_KILLED "killed"
#define ARG_KILLED_ARG 0
#define ARG_KILLED_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_KILLED_DESC "Killed (common to types: O,I)\n\t(default: false/live)"
#define ARG_KILLED_DEFAULT 0

// Message

#define ARG_RECIPIENT_KEY 'r'
#define ARG_RECIPIENT "recipient"
#define ARG_RECIPIENT_ARG "CALL"
#define ARG_RECIPIENT_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_RECIPIENT_DESC "Recipient (default: N0CALL)"
#define ARG_RECIPIENT_DEFAULT "N0CALL"

#define ARG_SEQUENCE_KEY ('s' + 1000)
#define ARG_SEQUENCE "sequence"
#define ARG_SEQUENCE_ARG "SEQUENCE"
#define ARG_SEQUENCE_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_SEQUENCE_DESC "Sequence number (default: 1)"
#define ARG_SEQUENCE_DEFAULT 1

#define ARG_ACK_KEY ('a' + 1000)
#define ARG_ACK "ack"
#define ARG_ACK_ARG 0
#define ARG_ACK_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_ACK_DESC "Send acknowledgement (default: false)"
#define ARG_ACK_DEFAULT 0

#define ARG_REJECT_KEY ('j' + 1000)
#define ARG_REJECT "rej"
#define ARG_REJECT_ARG 0
#define ARG_REJECT_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_REJECT_DESC "Send rejection (default: false)"
#define ARG_REJECT_DEFAULT 0

// KISS

#define ARG_KISS_KEY 'k'
#define ARG_KISS "kiss"
#define ARG_KISS_ARG 0
#define ARG_KISS_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_KISS_DESC "Use KISS format (default: false)"
#define ARG_KISS_DEFAULT 0

#define ARG_KISS_PORT_KEY 'p'
#define ARG_KISS_PORT "port"
#define ARG_KISS_PORT_ARG "PORT"
#define ARG_KISS_PORT_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_KISS_PORT_DESC "KISS port number (default: 0)"
#define ARG_KISS_PORT_DEFAULT 0

static struct argp_option options[] = {
    {0, 0, 0, 0, "Basic APRS packet parameters:"},
    {ARG_SOURCE, ARG_SOURCE_KEY, ARG_SOURCE_ARG, ARG_SOURCE_FLAGS, ARG_SOURCE_DESC},
    {ARG_DESTINATION, ARG_DESTINATION_KEY, ARG_DESTINATION_ARG, ARG_DESTINATION_FLAGS, ARG_DESTINATION_DESC},
    {ARG_TYPE, ARG_TYPE_KEY, ARG_TYPE_ARG, ARG_TYPE_FLAGS, ARG_TYPE_DESC},
    {0, 0, 0, 0, "Common parameters:"},
    {ARG_TEXT, ARG_TEXT_KEY, ARG_TEXT_ARG, ARG_TEXT_FLAGS, ARG_TEXT_DESC},
    {0, 0, 0, 0, "Position parameters:"},
    {ARG_LATITUDE, ARG_LATITUDE_KEY, ARG_LATITUDE_ARG, ARG_LATITUDE_FLAGS, ARG_LATITUDE_DESC},
    {ARG_LONGITUDE, ARG_LONGITUDE_KEY, ARG_LONGITUDE_ARG, ARG_LONGITUDE_FLAGS, ARG_LONGITUDE_DESC},
    {ARG_SYMBOL, ARG_SYMBOL_KEY, ARG_SYMBOL_ARG, ARG_SYMBOL_FLAGS, ARG_SYMBOL_DESC},
    {ARG_COMPRESSED, ARG_COMPRESSED_KEY, ARG_COMPRESSED_ARG, ARG_COMPRESSED_FLAGS, ARG_COMPRESSED_DESC},
    {0, 0, 0, 0, "Time parameters:"},
    {ARG_TIME, ARG_TIME_KEY, ARG_TIME_ARG, ARG_TIME_FLAGS, ARG_TIME_DESC},
    {ARG_TIME_UTC, ARG_TIME_UTC_KEY, ARG_TIME_UTC_ARG, ARG_TIME_UTC_FLAGS, ARG_TIME_UTC_DESC},
    {ARG_TIME_DHM, ARG_TIME_DHM_KEY, ARG_TIME_DHM_ARG, ARG_TIME_DHM_FLAGS, ARG_TIME_DHM_DESC},
    {0, 0, 0, 0, "Object/Item parameters:"},
    {ARG_NAME, ARG_NAME_KEY, ARG_NAME_ARG, ARG_NAME_FLAGS, ARG_NAME_DESC},
    {ARG_KILLED, ARG_KILLED_KEY, ARG_KILLED_ARG, ARG_KILLED_FLAGS, ARG_KILLED_DESC},
    {0, 0, 0, 0, "Position report parameters:"},
    {ARG_MESSAGING, ARG_MESSAGING_KEY, ARG_MESSAGING_ARG, ARG_MESSAGING_FLAGS, ARG_MESSAGING_DESC},
    {0, 0, 0, 0, "Message parameters:"},
    {ARG_RECIPIENT, ARG_RECIPIENT_KEY, ARG_RECIPIENT_ARG, ARG_RECIPIENT_FLAGS, ARG_RECIPIENT_DESC},
    {ARG_SEQUENCE, ARG_SEQUENCE_KEY, ARG_SEQUENCE_ARG, ARG_SEQUENCE_FLAGS, ARG_SEQUENCE_DESC},
    {ARG_ACK, ARG_ACK_KEY, ARG_ACK_ARG, ARG_ACK_FLAGS, ARG_ACK_DESC},
    {ARG_REJECT, ARG_REJECT_KEY, ARG_REJECT_ARG, ARG_REJECT_FLAGS, ARG_REJECT_DESC},
    {0, 0, 0, 0, "KISS parameters:"},
    {ARG_KISS, ARG_KISS_KEY, ARG_KISS_ARG, ARG_KISS_FLAGS, ARG_KISS_DESC},
    {ARG_KISS_PORT, ARG_KISS_PORT_KEY, ARG_KISS_PORT_ARG, ARG_KISS_PORT_FLAGS, ARG_KISS_PORT_DESC},
    {0}};

const char *argp_program_version = "aprscli dev";
static char doc[] = "Build APRS packets to KISS or TNC2 formats";

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    int arg_len = 0;
    aprscli_args_t *arguments = state->input;

    if (arg != NULL)
    {
        arg_len = strlen(arg);
    }

    switch (key)
    {
        // Basic

    case ARG_SOURCE_KEY:
        if (arg_len < 3)
        {
            fprintf(stderr, "Error: Source callsign is too short\n");
            argp_usage(state);
        }
        else if (arg_len > 9)
        {
            fprintf(stderr, "Error: Source callsign is too long\n");
            argp_usage(state);
        }
        arguments->source = arg;
        break;

    case ARG_DESTINATION_KEY:
        if (arg_len < 3)
        {
            fprintf(stderr, "Error: Destination callsign is too short\n");
            argp_usage(state);
        }
        else if (arg_len > 9)
        {
            fprintf(stderr, "Error: Destination callsign is too long\n");
            argp_usage(state);
        }
        arguments->destination = arg;
        break;

    case ARG_TYPE_KEY:
        if (arg_len != 1)
        {
            fprintf(stderr, "Error: Packet type must be exactly 1 character long\n");
            argp_usage(state);
        }
        arguments->type = arg[0];
        break;

    // Common

    case ARG_TEXT_KEY:
        arguments->text = arg;
        break;

    // Position

    case ARG_LATITUDE_KEY:
        arguments->latitude = atof(arg);
        break;

    case ARG_LONGITUDE_KEY:
        arguments->longitude = atof(arg);
        break;

    case ARG_SYMBOL_KEY:
        if (arg_len != 2)
        {
            fprintf(stderr, "Error: Symbol must be exactly 2 characters long\n");
            argp_usage(state);
        }
        arguments->symbol[0] = arg[0];
        arguments->symbol[1] = arg[1];
        break;

    case ARG_COMPRESSED_KEY:
        arguments->compressed = true;
        break;

    // Time

    case ARG_TIME_KEY:
        arguments->time = arg;
        break;

    case ARG_TIME_UTC_KEY:
        arguments->time_utc = true;
        break;

    case ARG_TIME_DHM_KEY:
        arguments->time_dhm = true;
        break;

    // Object/Item

    case ARG_NAME_KEY:
        arguments->name = arg;
        break;

    case ARG_KILLED_KEY:
        arguments->killed = true;
        break;

    // Position report

    case ARG_MESSAGING_KEY:
        arguments->messaging = true;
        break;

    // Message

    case ARG_RECIPIENT_KEY:
        if (arg_len < 3)
        {
            fprintf(stderr, "Error: Recipient callsign is too short\n");
            argp_usage(state);
        }
        else if (arg_len > 9)
        {
            fprintf(stderr, "Error: Recipient callsign is too long\n");
            argp_usage(state);
        }
        arguments->recipient = arg;
        break;

    case ARG_SEQUENCE_KEY:
        arguments->sequence_number = atoi(arg);
        break;
    
    case ARG_ACK_KEY:
        arguments->ack = true;
        break;
    
    case ARG_REJECT_KEY:
        arguments->rej = true;
        break;

    // KISS

    case ARG_KISS_KEY:
        arguments->kiss = true;
        break;

    case ARG_KISS_PORT_KEY:
        arguments->kiss_port = atoi(arg);
        break;

    case ARGP_KEY_ARG:
        fprintf(stderr, "Error: Positional arguments are not supported\n");
        argp_usage(state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = {options, parse_opt, 0, doc};

int aprscli_parse_args(int argc, char **argv, aprscli_args_t *args)
{
    args->source = ARG_SOURCE_DEFAULT;
    args->destination = ARG_DESTINATION_DEFAULT;
    args->type = ARG_TYPE_DEFAULT;
    args->symbol[0] = ARG_SYMBOL_DEFAULT[0];
    args->symbol[1] = ARG_SYMBOL_DEFAULT[1];
    args->text = ARG_TEXT_DEFAULT;
    args->time = ARG_TIME_DEFAULT;
    args->latitude = ARG_LATITUDE_DEFAULT;
    args->longitude = ARG_LONGITUDE_DEFAULT;
    args->compressed = ARG_COMPRESSED_DEFAULT;
    args->messaging = ARG_MESSAGING_DEFAULT;
    args->time_utc = ARG_TIME_UTC_DEFAULT;
    args->time_dhm = ARG_TIME_DHM_DEFAULT;
    args->kiss = ARG_KISS_DEFAULT;
    args->kiss_port = ARG_KISS_PORT_DEFAULT;

    return argp_parse(&argp, argc, argv, 0, 0, args);
}