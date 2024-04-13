#ifndef _MODEMSTUFF_AX25_CALL_H
#define _MODEMSTUFF_AX25_CALL_H

#include <modemstuff/modemstuff.h>

#define MS_AX25_MAX_CALLSIGN_LEN 6
#define MS_AX25_MAX_SSID_STRING_LEN 2
#define MS_AX25_MAX_CALLSIGN_STRING_LEN 10
#define MS_AX25_SSID_SEPARATOR '-'
#define MS_AX25_REPEATED_MARKER '*'

typedef struct ms_ax25_call
{
    char base[MS_AX25_MAX_CALLSIGN_LEN + 1];
    ms_byte ssid;
    ms_byte is_repeated;
} ms_ax25_call_t;

void ms_ax25_call_init(ms_ax25_call_t *call);

void ms_ax25_call_pack(ms_ax25_call_t *call, ms_byte *out, ms_byte is_last);
ms_byte ms_ax25_call_unpack(ms_ax25_call_t *call, const ms_byte *inp);

ms_byte ms_ax25_call_to_str(ms_ax25_call_t *call, char *out);
void ms_ax25_call_from_str(ms_ax25_call_t *call, const char *str);

#endif // _MODEMSTUFF_AX25_CALL_H