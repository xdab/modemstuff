#include <hamstuff/aprs/time.h>

#include <stdio.h>

void hs_aprs_time_init(hs_aprs_time_t *time)
{
    time->timestamp = 0;
    time->format = HS_APRS_TIME_FORMAT_DHM;
    time->zone = HS_APRS_TIME_ZONE_UTC;
}

int hs_aprs_time_pack(const hs_aprs_time_t *time, char *buf, int buf_len)
{
    struct tm *tm;
    int i;

    if (time->timestamp < 0)
    {
#ifdef VERBOSE
        fprintf(stderr, "Error: Invalid timestamp\n");
#endif
        return -1;
    }

    // Format HMS may only be used with UTC
    if (time->format == HS_APRS_TIME_FORMAT_HMS && time->zone != HS_APRS_TIME_ZONE_UTC)
    {
#ifdef VERBOSE
        fprintf(stderr, "Error: Invalid time format\n");
#endif
        return -1;
    }

    tm = localtime(&time->timestamp);

    if (tm == NULL)
    {
#ifdef VERBOSE
        fprintf(stderr, "Error: Invalid timestamp\n");
#endif
        return -1;
    }

    i = 0;

    switch (time->format)
    {
    case HS_APRS_TIME_FORMAT_DHM:
        i += snprintf(buf + i, buf_len - i, "%02d%02d%02d%c",
                      tm->tm_mday, tm->tm_hour, tm->tm_min,
                      (time->zone == HS_APRS_TIME_ZONE_UTC) ? 'z' : '/');
        break;

    case HS_APRS_TIME_FORMAT_HMS:
        i += snprintf(buf + i, buf_len - i, "%02d%02d%02dh",
                      tm->tm_hour, tm->tm_min, tm->tm_sec);
        break;

    default:
#ifdef VERBOSE
        fprintf(stderr, "Error: Invalid time format\n");
#endif
        return -1;
    }

    return i;
}