#include <hamstuff/aprs/aprs.h>

#include <stdio.h>
#include <string.h>

int hs_aprs_packet_init(hs_ax25_packet_t *packet, const char *src, const char *dest, const char *path, const char *info)
{
    char *token;
    char path_copy[256];

    hs_ax25_packet_init(packet);

    hs_ax25_call_from_str(&packet->src, src);
    hs_ax25_call_from_str(&packet->dest, dest);

    if (path)
    {
        path_copy[0] = 0;
        strncat(path_copy, path, sizeof(path_copy) - 1);
        token = strtok_r(path_copy, ",", NULL);
        while (token)
        {
            hs_ax25_call_from_str(&packet->repeaters[packet->repeater_count], token);
            token = strtok_r(0, ",", NULL);
            packet->repeater_count++;
        }
    }

    packet->control = 0x03;  // UI frame
    packet->protocol = 0xf0; // No layer 3 protocol

    packet->info_len = snprintf(packet->info, HS_AX25_MAX_INFO_LEN, "%s", info);

    return 0;
}