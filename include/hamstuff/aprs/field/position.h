#ifndef _HAMSTUFF_APRS_FIELD_POSITION_H
#define _HAMSTUFF_APRS_FIELD_POSITION_H

#include <stdbool.h>

typedef struct hs_aprs_position
{
    double latitude;
    double longitude;
    char symbol_table;
    char symbol_code;
    bool compressed;
} hs_aprs_position_t;

/**
 * Initialize an APRS position structure
 *
 * @param position The position structure to initialize
 */
void hs_aprs_position_init(hs_aprs_position_t *position);

/**
 * Pack an APRS position into a buffer
 *
 * @param position The position to pack
 * @param buf The buffer to pack the position into
 * @param buf_len The length of the buffer
 *
 * @return The number of bytes written to the buffer, or -1 on error
 */
int hs_aprs_position_pack(const hs_aprs_position_t *position, char *buf, int buf_len);

#endif // _HAMSTUFF_APRS_FIELD_POSITION_H