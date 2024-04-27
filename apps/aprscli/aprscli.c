#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"

#include <hamstuff/aprs/aprs.h>
#include <hamstuff/aprs/position.h>
#include <hamstuff/kiss.h>

int main(int argc, char **argv)
{
    aprscli_args_t arguments;
    hs_ax25_packet_t packet;
    hs_aprs_position_t position;
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
        position.latitude = arguments.latitude;
        position.longitude = arguments.longitude;
        position.symbol_table = arguments.symbol[0];
        position.symbol_code = arguments.symbol[1];
        position.compressed = arguments.compressed;
        position.messaging = false;
        if (arguments.text != NULL)
            strcpy(position.comment, arguments.text);
        else
            position.comment[0] = '\0';
        hs_aprs_position_pack(&position, packet_info, sizeof(packet_info));
        break;

    default:
        fprintf(stderr, "Error: Unsupported packet type %c\n", arguments.type);
        return EXIT_FAILURE;
    }

    hs_aprs_packet_init(&packet, arguments.source, arguments.destination, NULL, packet_info);

    if (arguments.confirm_before_sending)
    {
        packet_repr_length = hs_ax25_packet_pack_tnc2(&packet, packet_repr);
        fprintf(stderr, "Prepared packet in TNC2 format: %s\n", packet_repr);
        fprintf(stderr, "Press Enter to send the packet or interrupt to abort...");
        getchar();
    }

    if (arguments.kiss)
    {
        kiss_message.port = arguments.kiss_port;
        kiss_message.command = KISS_DATA_FRAME;
        kiss_message.data_length = hs_ax25_packet_pack(&packet, kiss_message.data, false);
        packet_repr_length = hs_kiss_encode(&kiss_message, packet_repr);
    }
    else if (packet_repr_length == 0)
        packet_repr_length = hs_ax25_packet_pack_tnc2(&packet, packet_repr);

    fwrite(packet_repr, 1, packet_repr_length, stdout);
    fflush(stdout);

    return EXIT_SUCCESS;
}
