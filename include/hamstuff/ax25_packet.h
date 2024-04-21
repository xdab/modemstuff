#ifndef _HAMSTUFF_AX25_FRAME_H
#define _HAMSTUFF_AX25_FRAME_H

#include <hamstuff/hamstuff.h>
#include <hamstuff/ax25_call.h>

#define HS_AX25_MAX_REPEATER_COUNT 8
#define HS_AX25_MAX_INFO_LEN 255

typedef struct hs_ax25_packet
{
    hs_ax25_call_t dest;
    hs_ax25_call_t src;

    hs_ax25_call_t repeaters[HS_AX25_MAX_REPEATER_COUNT];
    hs_byte repeater_count;

    hs_byte control;
    hs_byte protocol;

    hs_byte info[HS_AX25_MAX_INFO_LEN];
    hs_byte info_len;

} hs_ax25_packet_t;

void hs_ax25_packet_init(hs_ax25_packet_t *pkt);
int hs_ax25_packet_pack(hs_ax25_packet_t *pkt, hs_byte *out);
void hs_ax25_packet_unpack(hs_ax25_packet_t *pkt, const hs_byte *inp, const int inp_len);
int hs_ax25_packet_pack_tnc2(hs_ax25_packet_t *pkt, char *str);

#endif // _HAMSTUFF_AX25_FRAME_H