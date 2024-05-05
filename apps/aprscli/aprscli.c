#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "args.h"

#include <hamstuff/aprs/aprs.h>
#include <hamstuff/aprs/report/position.h>
#include <hamstuff/kiss.h>

static int prepare_position_report(aprscli_args_t *arguments, char *packet_info, long packet_info_len)
{
    hs_aprs_position_report_t report;
    struct tm tm;

    hs_aprs_position_report_init(&report);
    memset(&tm, 0, sizeof(tm));

    report.position.latitude = arguments->latitude;
    report.position.longitude = arguments->longitude;
    report.position.symbol_table = arguments->symbol[0];
    report.position.symbol_code = arguments->symbol[1];
    report.position.compressed = arguments->compressed;

    if (arguments->time != NULL)
    {
        report.time.format = arguments->time_dhm ? HS_APRS_TIME_FORMAT_DHM : HS_APRS_TIME_FORMAT_HMS;
        report.time.zone = arguments->time_utc ? HS_APRS_TIME_ZONE_UTC : HS_APRS_TIME_ZONE_LOCAL;

        // HMS may only be used with UTC
        if (report.time.format == HS_APRS_TIME_FORMAT_HMS && report.time.zone != HS_APRS_TIME_ZONE_UTC)
        {
            fprintf(stderr, "Error: HMS time format may only be used with UTC\n");
            return -1;
        }

        // Parse ISO 8601 time
        if (sscanf(arguments->time, "%d-%d-%dT%d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6)
        {
            fprintf(stderr, "Error: Failed to parse time\n");
            return -1;
        }

        // Adjust year and month
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;   // 0 = January
        tm.tm_isdst = -1; // Unknown

        // Convert to timestamp
        report.time.timestamp = mktime(&tm);
        if (report.time.timestamp <= 0)
        {
            fprintf(stderr, "Error: Failed to convert time to timestamp\n");
            return -1;
        }
    }

    if (arguments->text != NULL)
        strcpy(report.comment, arguments->text);

    report.messaging = arguments->messaging;

    if (hs_aprs_position_report_pack(&report, packet_info, packet_info_len) < 0)
    {
        fprintf(stderr, "Error: Failed to pack position report\n");
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    aprscli_args_t arguments;
    hs_ax25_packet_t packet;
    char packet_info[256] = {0};
    char packet_repr[512] = {0};
    int packet_repr_length = 0;
    hs_kiss_message_t kiss_message;

    if (aprscli_parse_args(argc, argv, &arguments) != 0)
        return EXIT_FAILURE;

    switch (arguments.type)
    {
    case APRSCLI_PACKET_TYPE_STATUS:
        // Prepare a status info field
        snprintf(packet_info, sizeof(packet_info), ">%s", arguments.text);
        break;

    case APRSCLI_PACKET_TYPE_POSITION_REPORT:
        if (prepare_position_report(&arguments, packet_info, sizeof(packet_info)) != 0)
            return EXIT_FAILURE;
        break;

    default:
        fprintf(stderr, "Error: Unsupported packet type %c\n", arguments.type);
        return EXIT_FAILURE;
    }

    hs_aprs_packet_init(&packet, arguments.source, arguments.destination, NULL, packet_info);

    if (arguments.kiss)
    {
        kiss_message.port = arguments.kiss_port;
        kiss_message.command = KISS_DATA_FRAME;
        kiss_message.data_length = hs_ax25_packet_pack(&packet, kiss_message.data, false);
        packet_repr_length = hs_kiss_encode(&kiss_message, packet_repr);
    }
    else
    {
        packet_repr_length = hs_ax25_packet_pack_tnc2(&packet, packet_repr);
    }

    fwrite(packet_repr, 1, packet_repr_length, stdout);
    fflush(stdout);

    return EXIT_SUCCESS;
}
