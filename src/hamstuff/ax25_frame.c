#include <hamstuff/ax25_frame.h>

void hs_ax25_frame_init(hs_ax25_frame_t *frame)
{
    hs_ax25_call_init(&frame->dest);
    hs_ax25_call_init(&frame->src);
    frame->repeater_count = 0;
    frame->control = 0;
    frame->protocol = 0;
    frame->info_len = 0;
}

int hs_ax25_frame_pack(hs_ax25_frame_t *frame, hs_byte *out)
{
    int i, j;

    i = 0;

    // Pack destination callsign
    hs_ax25_call_pack(&frame->dest, &out[i], 0);
    i += 7;

    // Pack source callsign
    hs_ax25_call_pack(&frame->src, &out[i], (frame->repeater_count == 0));
    i += 7;

    // Pack repeaters
    for (j = 0; j < frame->repeater_count; j++, i += 7)
        hs_ax25_call_pack(&frame->repeaters[j], &out[i], j == (frame->repeater_count - 1));

    // Pack control and protocol
    out[i++] = frame->control;
    out[i++] = frame->protocol;

    // Pack info
    for (j = 0; j < frame->info_len; i++, j++)
        out[i] = frame->info[j];

    // Calculate FCS
    unsigned short crc = 0x0000; // TODO: CRC16_X25_INIT

    // Pack FCS, big-endian
    out[i++] = crc >> 8;
    out[i++] = crc & 0xff;

    return i;
}

void hs_ax25_frame_unpack(hs_ax25_frame_t *frame, const hs_byte *inp, const int inp_len)
{
    int i, j;

    i = 0;

    // Unpack destination callsign
    hs_ax25_call_unpack(&frame->dest, &inp[i]);
    i += 7;

    // Unpack source callsign
    hs_byte is_last = hs_ax25_call_unpack(&frame->src, &inp[i]);
    i += 7;

    frame->repeater_count = 0;
    // If source is not last, unpack repeaters
    if (!is_last)
    {
        // Unpack repeaters
        while (frame->repeater_count < HS_AX25_MAX_REPEATER_COUNT)
        {
            hs_ax25_call_t *repeater = &frame->repeaters[frame->repeater_count];
            is_last = hs_ax25_call_unpack(repeater, &inp[i]);
            i += 7;
            if (is_last)
                break;
            frame->repeater_count++;
        }
    }

    // Unpack control and protocol
    frame->control = inp[i++];
    frame->protocol = inp[i++];

    // Unpack info
    for (j = 0; (i < inp_len - 2) && (j < HS_AX25_MAX_INFO_LEN); i++, j++)
        frame->info[j] = inp[i];

    frame->info_len = j;
}

int hs_ax25_frame_pack_tnc2(hs_ax25_frame_t *frame, char *str)
{
    int i, j;

    i = 0;

    // Pack source callsign
    i += hs_ax25_call_to_str(&frame->src, str + i);

    // Source-destination separator
    str[i++] = '>';

    // Pack destination callsign
    i += hs_ax25_call_to_str(&frame->dest, str + i);

    // Pack repeaters, if any
    for (j = 0; j < frame->repeater_count; j++)
    {
        str[i++] = ','; // Repeater separator
        i += hs_ax25_call_to_str(&frame->repeaters[j], str + i);
    }

    // Pack control and protocol
    str[i++] = ':';

    // Pack info
    for (j = 0; j < frame->info_len; j++)
        str[i++] = frame->info[j];

    // Null terminate result
    str[i++] = '\0';

    return i;
}