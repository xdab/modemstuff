#include <hamstuff/ax25/call.h>

void hs_ax25_call_init(hs_ax25_call_t *call)
{
    call->base[0] = '\0';
    call->ssid = 0;
    call->is_repeated = 0;
}

void hs_ax25_call_pack(hs_ax25_call_t *call, hs_byte *out, hs_byte is_last)
{
    int i;

    // Copy shifted base callsign
    i = 0;
    while (call->base[i] != '\0')
    {
        out[i] = call->base[i] << 1;
        i++;
    }

    // Pad with spaces
    while (i < HS_AX25_MAX_CALLSIGN_LEN)
    {
        out[i] = ' ' << 1;
        i++;
    }

    // Encode metadata
    out[HS_AX25_MAX_CALLSIGN_LEN] = 0b01100000; // Reserved bits are set to ones
    out[HS_AX25_MAX_CALLSIGN_LEN] |= call->is_repeated << 7;
    out[HS_AX25_MAX_CALLSIGN_LEN] |= call->ssid << 1;
    out[HS_AX25_MAX_CALLSIGN_LEN] |= is_last;
}

hs_byte hs_ax25_call_unpack(hs_ax25_call_t *call, const hs_byte *inp)
{
    int i;

    // Copy unshifted base callsign
    i = 0;
    while (i < HS_AX25_MAX_CALLSIGN_LEN)
    {
        call->base[i] = (inp[i] >> 1) & 0b01111111;
        if (call->base[i] == ' ')
            break;
        i++;
    }
    call->base[i] = '\0';

    // Extract metadata
    call->is_repeated = (inp[HS_AX25_MAX_CALLSIGN_LEN] >> 7) & 1;
    call->ssid = (inp[HS_AX25_MAX_CALLSIGN_LEN] >> 1) & 0x0f;
    hs_byte is_last = inp[HS_AX25_MAX_CALLSIGN_LEN] & 1;

    return is_last;
}

hs_byte hs_ax25_call_to_str(hs_ax25_call_t *call, char *out)
{
    int i;

    // Copy callsign
    i = 0;
    while ((i < HS_AX25_MAX_CALLSIGN_LEN) && (call->base[i] != '\0'))
    {
        out[i] = call->base[i];
        i++;
    }

    // Add SSID separator
    if (call->ssid != 0)
    {
        out[i] = HS_AX25_SSID_SEPARATOR;
        i++;

        if (call->ssid >= 10)
        {
            out[i] = '0' + (call->ssid / 10);
            i++;
            out[i] = '0' + (call->ssid % 10);
            i++;
        }
        else
        {
            out[i] = '0' + call->ssid;
            i++;
        }
    }

    // Add repeated marker
    if (call->is_repeated)
    {
        out[i] = HS_AX25_REPEATED_MARKER;
        i++;
    }

    out[i] = '\0';

    return i;
}

void hs_ax25_call_from_str(hs_ax25_call_t *call, const char *str)
{
    int i;

    // Extract base callsign
    i = 0;
    while ((i < HS_AX25_MAX_CALLSIGN_LEN) && (str[i] != '\0') && (str[i] != HS_AX25_SSID_SEPARATOR))
    {
        call->base[i] = str[i];
        i++;
    }
    call->base[i] = '\0';

    // Check for SSID
    call->ssid = 0;
    if ((i < HS_AX25_MAX_CALLSIGN_STRING_LEN) && (str[i] == HS_AX25_SSID_SEPARATOR))
    {
        // Extract SSID if present
        i++;
        int j = 0; // index into SSID substring
        while ((str[i] >= '0') && (str[i] <= '9') && (j < HS_AX25_MAX_SSID_STRING_LEN))
        {
            call->ssid = call->ssid * 10 + (str[i] - '0');
            i++;
            j++;
        }
    }

    // Check for repeated marker
    call->is_repeated = (i < HS_AX25_MAX_CALLSIGN_STRING_LEN) && (str[i] == HS_AX25_REPEATED_MARKER);
}