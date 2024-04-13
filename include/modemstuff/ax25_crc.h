#ifndef _MS_AX25_CRC_H
#define _MS_AX25_CRC_H

unsigned short ms_ax25_crc16_init();
unsigned short ms_ax25_crc16_update(unsigned char byte, unsigned short crc);
unsigned short ms_ax25_crc16_finalize(unsigned short crc);

#endif // _MS_AX25_CRC_H