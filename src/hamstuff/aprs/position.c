#include <hamstuff/aprs/position.h>

#include <stdio.h>

int hs_aprs_position_pack(const hs_aprs_position_t *position, char *buf, int buf_len)
{
    int i;
    double degrees;
    double minutes;
    char direction;

    if (position->latitude < -90.0 || position->latitude > 90.0)
    {
        fprintf(stderr, "Error: Invalid latitude\n");
        return -1;
    }

    if (position->longitude < -180.0 || position->longitude > 180.0)
    {
        fprintf(stderr, "Error: Invalid longitude\n");
        return -1;
    }

    if (position->symbol_table != '/' && position->symbol_table != '\\')
    {
        fprintf(stderr, "Error: Invalid symbol table\n");
        return -1;
    }

    if (position->symbol_code < 0x20 || position->symbol_code > 0x7e)
    {
        fprintf(stderr, "Error: Invalid symbol code\n");
        return -1;
    }

    if (buf_len < 20)
    {
        fprintf(stderr, "Error: Buffer too small\n");
        return -1;
    }

    i = 0; // Index into the buffer

    // Position report header
    buf[i++] = (position->messaging) ? '=' : '!';

    // Latitude
    degrees = position->latitude;
    if (degrees < 0)
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
    if (degrees < 0)
    {
        degrees = -degrees;
        direction = 'W';
    }
    else
        direction = 'E';
    minutes = (degrees - (int)degrees) * 60.0;
    i += sprintf(&buf[i], "%03d%05.2f%c", (int)position->longitude, (position->longitude - (int)position->longitude) * 60.0, direction);

    // Symbol code
    buf[i++] = position->symbol_code;

    // Comment
    if (position->comment[0] != '\0')
    {
        i += snprintf(&buf[i], buf_len - i, "%s", position->comment);
    }

    return i;
}