#ifndef _HAMSTUFF_AX25_CRC_H
#define _HAMSTUFF_AX25_CRC_H

#include <hamstuff/hamstuff.h>

typedef unsigned short hs_crc16_t;

hs_crc16_t hs_ax25_crc16_init();
hs_crc16_t hs_ax25_crc16_update(hs_byte byte, hs_crc16_t crc);
hs_crc16_t hs_ax25_crc16_finalize(hs_crc16_t crc);

#endif // _HAMSTUFF_AX25_CRC_H