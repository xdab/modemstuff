#include <hamstuff/aprs/position.h>

#include <math.h>
#include <stdio.h>

static int _hs_aprs_base91_encode(unsigned long value, char *buf, int buf_len);
static int _hs_aprs_compressed_position_pack(const hs_aprs_position_t *position, char *buf, int buf_len);
static int _hs_aprs_uncompressed_position_pack(const hs_aprs_position_t *position, char *buf, int buf_len);

int hs_aprs_position_pack(const hs_aprs_position_t *position, char *buf, int buf_len)
{
    int i;

    if (position->latitude < -90.0 || position->latitude > 90.0)
    {
#ifdef VERBOSE
        fprintf(stderr, "Error: Invalid latitude\n");
#endif
        return -1;
    }

    if (position->longitude < -180.0 || position->longitude > 180.0)
    {
#ifdef VERBOSE
        fprintf(stderr, "Error: Invalid longitude\n");
#endif
        return -1;
    }

    if (position->symbol_table != '/' && position->symbol_table != '\\')
    {
#ifdef VERBOSE
        fprintf(stderr, "Error: Invalid symbol table\n");
#endif
        return -1;
    }

    if (position->symbol_code < 0x20 || position->symbol_code > 0x7e)
    {
#ifdef VERBOSE
        fprintf(stderr, "Error: Invalid symbol code\n");
#endif
        return -1;
    }

    i = 0;

    // Position report header
    buf[i++] = (position->messaging) ? '=' : '!';

    if (position->compressed)
        // Compressed position report
        i = _hs_aprs_compressed_position_pack(position, buf + i, buf_len - i);
    else
        // Uncompressed position report
        i = _hs_aprs_uncompressed_position_pack(position, buf + i, buf_len - i);

    // Comment
    if (position->comment[0] != '\0')
        i += snprintf(&buf[i], buf_len - i, "%s", position->comment);

    return i;
}

static int _hs_aprs_compressed_position_pack(const hs_aprs_position_t *position, char *buf, int buf_len)
{
    int i, j;
    long units;
    char base91_buf[16] = {0};

    i = 0; // Number of characters written

    // Symbol table
    buf[i++] = position->symbol_table;

    // Latitude
    units = (long)floor((90.0 - position->latitude) * 380926.0);
    for (j = _hs_aprs_base91_encode(units, base91_buf, sizeof(base91_buf)) - 1; j >= 0; j--)
        buf[i++] = base91_buf[j];

    // Longitude
    units = (long)floor((180.0 + position->longitude) * 190463.0);
    for (j = _hs_aprs_base91_encode(units, base91_buf, sizeof(base91_buf)) - 1; j >= 0; j--)
        buf[i++] = base91_buf[j];

    // Symbol code
    buf[i++] = position->symbol_code;

    // Three spaces
    buf[i++] = ' ';
    buf[i++] = ' ';
    buf[i++] = ' ';

    return i;
}

static int _hs_aprs_uncompressed_position_pack(const hs_aprs_position_t *position, char *buf, int buf_len)
{
    int i;
    double degrees;
    double minutes;
    char direction;

    i = 0; // Number of characters written

    // Latitude
    degrees = position->latitude;
    if (degrees < 0.0)
    {
        degrees = -degrees;
        direction = 'S';
    }
    else
        direction = 'N';
    minutes = (degrees - (int)degrees) * 60.0;
    i += sprintf(&buf[i], "%02d%05.2f%c", (int)degrees, minutes, direction);

    // Symbol table
    buf[i++] = position->symbol_table;

    // Longitude
    degrees = position->longitude;
    if (degrees < 0.0)
    {
        degrees = -degrees;
        direction = 'W';
    }
    else
        direction = 'E';
    minutes = (degrees - (int)degrees) * 60.0;
    i += sprintf(&buf[i], "%03d%05.2f%c", (int)degrees, minutes, direction);

    // Symbol code
    buf[i++] = position->symbol_code;

    return i;
}

static int _hs_aprs_base91_encode(unsigned long value, char *buf, int buf_len)
{
    int i;

    i = 0; // Number of characters written

    while (value > 0 && i < buf_len)
    {
        buf[i++] = (value % 91) + 33;
        value = value / 91;
    }

    return i;
}