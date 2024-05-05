#ifndef _HAMSTUFF_APRS_APRS_H
#define _HAMSTUFF_APRS_APRS_H

#include <hamstuff/ax25/packet.h>

int hs_aprs_packet_init(hs_ax25_packet_t *packet, const char *src, const char *dest, const char *path, const char *info);

#endif // _HAMSTUFF_APRS_APRS_H