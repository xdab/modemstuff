#include <hamstuff/aprs/report/position.h>

#include <math.h>
#include <stdio.h>

void hs_aprs_position_report_init(hs_aprs_position_report_t *report)
{
    hs_aprs_time_init(&report->time);
    hs_aprs_position_init(&report->position);
    report->comment[0] = 0;
    report->messaging = false;
}

int hs_aprs_position_report_pack(const hs_aprs_position_report_t *report, char *buf, int buf_len)
{
    int i, r;

    i = 0;

    // Position report header
    buf[i++] = (report->time.timestamp > 0)
                   ? ((report->messaging) ? '@' : '/')
                   : ((report->messaging) ? '=' : '!');

    // Timestamp
    if (report->time.timestamp > 0)
    {
        if ((r = hs_aprs_time_pack(&report->time, buf + i, buf_len - i)) < 0)
        {
#ifdef VERBOSE
            fprintf(stderr, "Error: Failed to pack timestamp\n");
#endif
            return -1;
        }
        else
            i += r;
    }

    // Position
    if ((r = hs_aprs_position_pack(&report->position, buf + i, buf_len - i)) < 0)
    {
#ifdef VERBOSE
        fprintf(stderr, "Error: Unable to pack position\n");
#endif
        return -1;
    }
    else
        i += r;

    // Comment
    if (report->comment[0] > 0)
        i += snprintf(buf + i, buf_len - i, "%s", report->comment);

    return i;
}
