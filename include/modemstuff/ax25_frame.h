#ifndef _MODEMSTUFF_AX25_FRAME_H
#define _MODEMSTUFF_AX25_FRAME_H

#include <modemstuff/modemstuff.h>
#include <modemstuff/ax25_call.h>

#define MS_AX25_MAX_REPEATER_COUNT 8
#define MS_AX25_MAX_INFO_LEN 255

typedef struct ms_ax25_frame
{
    ms_ax25_call_t dest;
    ms_ax25_call_t src;

    ms_ax25_call_t repeaters[MS_AX25_MAX_REPEATER_COUNT];
    ms_byte repeater_count;

    ms_byte control;
    ms_byte protocol;

    ms_byte info[MS_AX25_MAX_INFO_LEN];
    ms_byte info_len;

} ms_ax25_frame_t;

void ms_ax25_frame_init(ms_ax25_frame_t *frame);
int ms_ax25_frame_pack(ms_ax25_frame_t *frame, ms_byte *out);
void ms_ax25_frame_unpack(ms_ax25_frame_t *frame, const ms_byte *inp, const int inp_len);
int ms_ax25_frame_pack_tnc2(ms_ax25_frame_t *frame, char *str);

#endif