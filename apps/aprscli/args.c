#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

// Positional

#define POSITIONAL_ARG_COUNT 1

#define ARG_HOSTPORT_POSITION 0

// Basic

#define ARG_SOURCE_KEY 's'
#define ARG_SOURCE "src"
#define ARG_SOURCE_ARG "SRC"
#define ARG_SOURCE_FLAGS (0)
#define ARG_SOURCE_DESC "Source callsign"
#define ARG_SOURCE_DEFAULT "N0CALL"

#define ARG_DESTINATION_KEY 'd'
#define ARG_DESTINATION "dest"
#define ARG_DESTINATION_ARG "DEST"
#define ARG_DESTINATION_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_DESTINATION_DESC "Destination callsign (default: APRS)"
#define ARG_DESTINATION_DEFAULT "APRS"

#define ARG_TYPE_KEY 't'
#define ARG_TYPE "type"
#define ARG_TYPE_ARG "TYPE"
#define ARG_TYPE_FLAGS 0
#define ARG_TYPE_DESC "Type of APRS packet\n\tP = Position report\n\tS = Status"
#define ARG_TYPE_DEFAULT 0

// Common

#define ARG_TEXT_KEY 'x'
#define ARG_TEXT "text"
#define ARG_TEXT_ARG "TEXT"
#define ARG_TEXT_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_TEXT_DESC "Text (common to types: P,S)\n\t(default: No text)"

// Position report

#define ARG_LATITUDE_KEY 'l'
#define ARG_LATITUDE "lat"
#define ARG_LATITUDE_ARG "LAT"
#define ARG_LATITUDE_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_LATITUDE_DESC "Latitude in decimal degrees (default: 0.0)"
#define ARG_LATITUDE_DEFAULT 0.0

#define ARG_LONGITUDE_KEY 'L'
#define ARG_LONGITUDE "lon"
#define ARG_LONGITUDE_ARG "LON"
#define ARG_LONGITUDE_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_LONGITUDE_DESC "Longitude in decimal degrees (default: 0.0)"
#define ARG_LONGITUDE_DEFAULT 0.0

#define ARG_COMPRESSED_KEY 'c'
#define ARG_COMPRESSED "compressed"
#define ARG_COMPRESSED_ARG 0
#define ARG_COMPRESSED_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_COMPRESSED_DESC "Compressed position report (default: false)"
#define ARG_COMPRESSED_DEFAULT 0

// Extra

#define ARG_CONFIRM_KEY 'f'
#define ARG_CONFIRM "confirm"
#define ARG_CONFIRM_ARG 0
#define ARG_CONFIRM_FLAGS (OPTION_ARG_OPTIONAL)
#define ARG_CONFIRM_DESC "Confirm before sending (default: false)"
#define ARG_CONFIRM_DEFAULT 0

static struct argp_option options[] = {
    {0, 0, 0, 0, "Basic APRS packet parameters:"},
    {ARG_SOURCE, ARG_SOURCE_KEY, ARG_SOURCE_ARG, ARG_SOURCE_FLAGS, ARG_SOURCE_DESC},
    {ARG_DESTINATION, ARG_DESTINATION_KEY, ARG_DESTINATION_ARG, ARG_DESTINATION_FLAGS, ARG_DESTINATION_DESC},
    {ARG_TYPE, ARG_TYPE_KEY, ARG_TYPE_ARG, ARG_TYPE_FLAGS, ARG_TYPE_DESC},
    {0, 0, 0, 0, "Common parameters:"},
    {ARG_TEXT, ARG_TEXT_KEY, ARG_TEXT_ARG, ARG_TEXT_FLAGS, ARG_TEXT_DESC},
    {0, 0, 0, 0, "Position report parameters:"},
    {ARG_LATITUDE, ARG_LATITUDE_KEY, ARG_LATITUDE_ARG, ARG_LATITUDE_FLAGS, ARG_LATITUDE_DESC},
    {ARG_LONGITUDE, ARG_LONGITUDE_KEY, ARG_LONGITUDE_ARG, ARG_LONGITUDE_FLAGS, ARG_LONGITUDE_DESC},
    {ARG_COMPRESSED, ARG_COMPRESSED_KEY, ARG_COMPRESSED_ARG, ARG_COMPRESSED_FLAGS, ARG_COMPRESSED_DESC},
    {0, 0, 0, 0, "Extras:"},
    {ARG_CONFIRM, ARG_CONFIRM_KEY, ARG_CONFIRM_ARG, ARG_CONFIRM_FLAGS, ARG_CONFIRM_DESC},
    {0}};

const char *argp_program_version = "aprscli dev";

static char doc[] =
    "Build and send APRS packets to TCP/IP KISS TNCs.";

static char args_doc[] = "HOST:PORT";

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    aprscli_args_t *arguments = state->input;

    switch (key)
    {
    case ARG_SOURCE_KEY:
        arguments->source = arg;
        break;

    case ARG_DESTINATION_KEY:
        arguments->destination = arg;
        break;

    case ARG_TYPE_KEY:
        arguments->type = arg[0];
        break;

    case ARG_TEXT_KEY:
        arguments->text = arg;
        break;

    case ARG_LATITUDE_KEY:
        arguments->latitude = atof(arg);
        break;

    case ARG_LONGITUDE_KEY:
        arguments->longitude = atof(arg);
        break;

    case ARG_COMPRESSED_KEY:
        arguments->compressed = true;
        break;

    case ARG_CONFIRM_KEY:
        arguments->confirm_before_sending = true;
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= POSITIONAL_ARG_COUNT)
            // Too many arguments, print usage and exit
            argp_usage(state);
        else if (state->arg_num == ARG_HOSTPORT_POSITION)
            arguments->host_port = arg;
        break;

    case ARGP_KEY_END:
        if (state->arg_num < POSITIONAL_ARG_COUNT)
            // Not enough arguments, print usage and exit
            argp_usage(state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int aprscli_parse_args(int argc, char **argv, aprscli_args_t *args)
{
    args->host_port = NULL;
    args->source = ARG_SOURCE_DEFAULT;
    args->destination = ARG_DESTINATION_DEFAULT;
    args->type = ARG_TYPE_DEFAULT;
    args->text = "";
    args->latitude = ARG_LATITUDE_DEFAULT;
    args->longitude = ARG_LONGITUDE_DEFAULT;
    args->compressed = ARG_COMPRESSED_DEFAULT;

    return argp_parse(&argp, argc, argv, 0, 0, args);
}