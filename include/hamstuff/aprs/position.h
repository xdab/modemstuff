#ifndef _HAMSTUFF_APRS_POSITION_H
#define _HAMSTUFF_APRS_POSITION_H

#include <stdbool.h>

typedef struct hs_aprs_position
{
    double latitude;
    double longitude;
    char symbol_table;
    char symbol_code;
    char comment[256];
    bool compressed;
    bool messaging;
} hs_aprs_position_t;

/**
 * Pack an APRS position report into a buffer
 *
 * @param position The position report to pack
 * @param buf The buffer to pack the position report into
 * @param buf_len The length of the buffer
 *
 * @return The number of bytes written to the buffer, or -1 on error
 */
int hs_aprs_position_pack(const hs_aprs_position_t *position, char *buf, int buf_len);

#endif // _HAMSTUFF_APRS_POSITION_H