#ifndef _HAMSTUFF_APRS_REPORT_OBJECT_H
#define _HAMSTUFF_APRS_REPORT_OBJECT_H

#include <hamstuff/aprs/field/position.h>

typedef struct hs_aprs_object_report
{
    hs_aprs_position_t position;
} hs_aprs_object_report_t;

#endif // _HAMSTUFF_APRS_REPORT_OBJECT_H