#include <stdio.h>
#include <stdlib.h>

#include "args.h"

#include <hamstuff/ax25_packet.h>
#include <hamstuff/kiss.h>
#include <netstuff/netclient.h>

int main(int argc, char **argv)
{
    aprscli_args_t arguments;
    hs_ax25_packet_t packet;
    char tnc2[512];
    char kiss[512];
    hs_kiss_message_t kiss_message;
    int kiss_message_length;
    ns_client_t client;
    char host[128];
    int port;

    if (aprscli_parse_args(argc, argv, &arguments) != 0)
        return EXIT_FAILURE;

    hs_ax25_packet_init(&packet);
    packet.control = 0x03;  // UI frame
    packet.protocol = 0xf0; // No layer 3 protocol

    hs_ax25_call_from_str(&packet.src, arguments.source);
    fprintf(stderr, "Source = %s\n", arguments.source);

    hs_ax25_call_from_str(&packet.dest, arguments.destination);
    fprintf(stderr, "Destination = %s\n", arguments.destination);

    if (arguments.type == APRSCLI_PACKET_TYPE_STATUS)
    {
        packet.info_len = sprintf(packet.info, ">%s", arguments.text);
        fprintf(stderr, "Status = %s\n", arguments.text);
    }

    hs_ax25_packet_pack_tnc2(&packet, tnc2);
    fprintf(stderr, "%s\n", tnc2);

    kiss_message.port = 0;
    kiss_message.command = KISS_DATA_FRAME;
    kiss_message.data_length = hs_ax25_packet_pack(&packet, kiss_message.data);
    kiss_message_length = hs_kiss_encode(&kiss_message, kiss);

    if (ns_client_init(&client))
    {
        fprintf(stderr, "Error: ns_client_init() failed\n");
        return EXIT_FAILURE;
    }

    sscanf(arguments.host_port, "%127[^:]:%d", host, &port);

    fprintf(stderr, "Host = %s\n", host);
    fprintf(stderr, "Port = %d\n", port);

    if (ns_client_connect(&client, host, port))
    {
        fprintf(stderr, "Error: ns_client_connect() failed\n");
        return EXIT_FAILURE;
    }

    if (ns_client_send(&client, kiss, kiss_message_length))
    {
        fprintf(stderr, "Error: ns_client_send() failed\n");
        return EXIT_FAILURE;
    }

    ns_client_destroy(&client);

    fprintf(stderr, "Packet sent\n");

    return EXIT_SUCCESS;
}
