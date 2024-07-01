#include <hamstuff/ax25/packet.h>
#include <hamstuff/ax25/crc.h>

void hs_ax25_packet_init(hs_ax25_packet_t *pkt)
{
    hs_ax25_call_init(&pkt->dest);
    hs_ax25_call_init(&pkt->src);
    pkt->repeater_count = 0;
    pkt->control = 0;
    pkt->protocol = 0;
    pkt->info_len = 0;
}

int hs_ax25_packet_pack(hs_ax25_packet_t *pkt, hs_byte *out, bool include_fcs)
{
    int i, j;

    i = 0;

    // Pack destination callsign
    hs_ax25_call_pack(&pkt->dest, &out[i], 0);
    i += 7;

    // Pack source callsign
    hs_ax25_call_pack(&pkt->src, &out[i], (pkt->repeater_count == 0));
    i += 7;

    // Pack repeaters
    for (j = 0; j < pkt->repeater_count; j++, i += 7)
        hs_ax25_call_pack(&pkt->repeaters[j], &out[i], j == (pkt->repeater_count - 1));

    // Pack control and protocol
    out[i++] = pkt->control;
    out[i++] = pkt->protocol;

    // Pack info
    for (j = 0; j < pkt->info_len; i++, j++)
        out[i] = pkt->info[j];

    if (include_fcs)
    {
        // Calculate and pack FCS
        hs_crc16_t crc = hs_ax25_crc16_init();
        for (j = 0; j < i; j++)
            crc = hs_ax25_crc16_update(out[j], crc);
        crc = hs_ax25_crc16_finalize(crc);
        out[i++] = (crc & 0xff);
        out[i++] = (crc >> 8);
    }

    return i;
}

void hs_ax25_packet_unpack(hs_ax25_packet_t *pkt, const hs_byte *inp, const int inp_len)
{
    int i, j;

    i = 0;

    // Unpack destination callsign
    hs_ax25_call_unpack(&pkt->dest, &inp[i]);
    i += 7;

    // Unpack source callsign
    hs_byte is_last = hs_ax25_call_unpack(&pkt->src, &inp[i]);
    i += 7;

    // Unpack repeaters
    pkt->repeater_count = 0;
    while (!is_last && pkt->repeater_count < HS_AX25_MAX_REPEATER_COUNT)
    {
        hs_ax25_call_t *repeater = &pkt->repeaters[pkt->repeater_count];
        is_last = hs_ax25_call_unpack(repeater, &inp[i]);
        pkt->repeater_count++;
        i += 7;
        if (is_last)
            break;
    }

    // Unpack control and protocol
    pkt->control = inp[i++];
    pkt->protocol = inp[i++];

    // Unpack info
    for (j = 0; (i < inp_len) && (j < HS_AX25_MAX_INFO_LEN); i++, j++)
        pkt->info[j] = inp[i];

    pkt->info_len = j;
}

int hs_ax25_packet_pack_tnc2(hs_ax25_packet_t *pkt, char *str)
{
    int i, j;

    i = 0;

    // Pack source callsign
    i += hs_ax25_call_to_str(&pkt->src, str + i);

    // Source-destination separator
    str[i++] = '>';

    // Pack destination callsign
    i += hs_ax25_call_to_str(&pkt->dest, str + i);

    // Pack repeaters, if any
    for (j = 0; j < pkt->repeater_count; j++)
    {
        str[i++] = ','; // Repeater separator
        i += hs_ax25_call_to_str(&pkt->repeaters[j], str + i);
    }

    // Info separator
    str[i++] = ':';

    // Pack info
    for (j = 0; j < pkt->info_len; j++)
        str[i++] = pkt->info[j];

    // Null terminate result
    str[i++] = '\0';

    return i;
}