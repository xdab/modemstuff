#ifndef _HAMSTUFF_APRS_POSITION_REPORT_H
#define _HAMSTUFF_APRS_POSITION_REPORT_H

#include <hamstuff/aprs/field/time.h>
#include <hamstuff/aprs/field/position.h>

#include <stdbool.h>

typedef struct hs_aprs_position_report
{
    hs_aprs_time_t time;
    hs_aprs_position_t position;
    char comment[256];
    bool messaging;
} hs_aprs_position_report_t;

/**
 * Initialize an APRS position report structure
 *
 * @param report The position report structure to initialize
 */
void hs_aprs_position_report_init(hs_aprs_position_report_t *report);

/**
 * Pack an APRS position report into a buffer
 *
 * @param report The position report to pack
 * @param buf The buffer to pack the position report into
 * @param buf_len The length of the buffer
 *
 * @return The number of bytes written to the buffer, or -1 on error
 */
int hs_aprs_position_report_pack(const hs_aprs_position_report_t *report, char *buf, int buf_len);

#endif // _HAMSTUFF_APRS_POSITION_REPORT_H