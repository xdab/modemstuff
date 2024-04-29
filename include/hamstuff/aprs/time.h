#ifndef _HAMSTUFF_APRS_TIME_H
#define _HAMSTUFF_APRS_TIME_H

#include <time.h>

typedef enum
{
    HS_APRS_TIME_FORMAT_DHM = 0,
    HS_APRS_TIME_FORMAT_HMS = 1
} hs_aprs_time_format_t;

typedef enum
{
    HS_APRS_TIME_ZONE_UTC = 0,
    HS_APRS_TIME_ZONE_LOCAL = 1
} hs_aprs_time_zone_t;

typedef struct hs_aprs_time
{
    time_t timestamp;
    hs_aprs_time_format_t format;
    hs_aprs_time_zone_t zone;
} hs_aprs_time_t;

/**
 * Initialize an APRS time structure
 *
 * @param time The time structure to initialize
 */
void hs_aprs_time_init(hs_aprs_time_t *time);

/**
 * Pack an APRS time into a buffer
 *
 * @param time The time to pack
 * @param buf The buffer to pack the time into
 * @param buf_len The length of the buffer
 *
 * @return The number of bytes written to the buffer, or -1 on error
 */
int hs_aprs_time_pack(const hs_aprs_time_t *time, char *buf, int buf_len);

#endif // _HAMSTUFF_APRS_TIME_H