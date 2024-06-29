#include <hamstuff/aprs/report/status.h>
#include <stdio.h>

void hs_aprs_status_report_init(hs_aprs_status_report_t *report)
{
    hs_aprs_time_init(&report->time);
    report->status[0] = '\0';
}

int hs_aprs_status_report_pack(hs_aprs_status_report_t *report, char *buffer, long buffer_len)
{
    int i, r;

    if (buffer_len < 1)
        return -1;

    i = 0;
    buffer[i++] = '>';

    if (report->time.timestamp > 0)
    {
        if ((r = hs_aprs_time_pack(&report->time, buffer + i, buffer_len - i)) < 0)
        {
#ifdef VERBOSE
            fprintf(stderr, "Error: Failed to pack timestamp\n");
#endif
            return -1;
        }
        else
            i += r;
    }

    i += snprintf(buffer + i, buffer_len - i, "%s", report->status);

    return i;
}