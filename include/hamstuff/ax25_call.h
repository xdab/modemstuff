#ifndef _HAMSTUFF_AX25_CALL_H
#define _HAMSTUFF_AX25_CALL_H

#include <hamstuff/hamstuff.h>

#define HS_AX25_MAX_CALLSIGN_LEN 6
#define HS_AX25_MAX_SSID_STRING_LEN 2
#define HS_AX25_MAX_CALLSIGN_STRING_LEN 10
#define HS_AX25_SSID_SEPARATOR '-'
#define HS_AX25_REPEATED_MARKER '*'

typedef struct hs_ax25_call
{
    char base[HS_AX25_MAX_CALLSIGN_LEN + 1];
    hs_byte ssid;
    hs_byte is_repeated;
} hs_ax25_call_t;

void hs_ax25_call_init(hs_ax25_call_t *call);

void hs_ax25_call_pack(hs_ax25_call_t *call, hs_byte *out, hs_byte is_last);
hs_byte hs_ax25_call_unpack(hs_ax25_call_t *call, const hs_byte *inp);

hs_byte hs_ax25_call_to_str(hs_ax25_call_t *call, char *out);
void hs_ax25_call_from_str(hs_ax25_call_t *call, const char *str);

#endif // _HAMSTUFF_AX25_CALL_H