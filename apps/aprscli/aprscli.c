#include <stdio.h>
#include <stdlib.h>

#include "args.h"

#include <hamstuff/aprs/aprs.h>
#include <hamstuff/kiss.h>
#include <netstuff/send.h>

int main(int argc, char **argv)
{
    aprscli_args_t arguments;
    hs_ax25_packet_t packet;
    char packet_info[256];
    char packet_tnc2_repr[512];
    hs_kiss_message_t kiss_message;
    char kiss_message_buf[512];
    int kiss_message_length;
    int ret;
    char host[128];
    int port;

    if (aprscli_parse_args(argc, argv, &arguments) != 0)
        return EXIT_FAILURE;

    if (sscanf(arguments.host_port, "%127[^:]:%d", host, &port) != 2)
    {
        fprintf(stderr, "Error: Invalid host:port format\n");
        return EXIT_FAILURE;
    }

    if (arguments.type == APRSCLI_PACKET_TYPE_STATUS)
    {
        // Prepare a status info field
        snprintf(packet_info, sizeof(packet_info), ">%s", arguments.text);
    }

    hs_aprs_packet_init(&packet, arguments.source, arguments.destination, NULL, packet_info);

    if (arguments.confirm_before_sending)
    {
        hs_ax25_packet_pack_tnc2(&packet, packet_tnc2_repr);
        printf("Prepared packet in TNC2 format: %s\n", packet_tnc2_repr);
        printf("Press Enter to send the packet or interrupt to abort...");
        getchar();
    }

    kiss_message.port = 0;
    kiss_message.command = KISS_DATA_FRAME;
    kiss_message.data_length = hs_ax25_packet_pack(&packet, kiss_message.data, false);
    kiss_message_length = hs_kiss_encode(&kiss_message, kiss_message_buf);

    if (ns_send(host, port, kiss_message_buf, kiss_message_length))
    {
        fprintf(stderr, "Error: ns_send() failed\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
