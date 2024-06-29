#ifndef _HAMSTUFF_APRS_REPORT_STATUS_H
#define _HAMSTUFF_APRS_REPORT_STATUS_H

#include <hamstuff/aprs/field/time.h>

typedef struct hs_aprs_status_report
{
    hs_aprs_time_t time;
    char status[256];
} hs_aprs_status_report_t;

/**
 * @brief Initialize an APRS status report structure
 * 
 * @param report The structure to initialize
 */
void hs_aprs_status_report_init(hs_aprs_status_report_t *report);

/**
 * @brief Pack an APRS status report into a buffer
 * 
 * @param report The report to pack
 * @param buffer The buffer to pack the report into
 * @param buffer_len The length of the buffer
 * @return Number of bytes written to the buffer, or -1 on error
 */
int hs_aprs_status_report_pack(hs_aprs_status_report_t *report, char *buffer, long buffer_len);

#endif // _HAMSTUFF_APRS_REPORT_STATUS_H