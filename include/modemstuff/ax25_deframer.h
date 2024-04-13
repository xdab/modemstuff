#ifndef _MODEMSTUFF_AX25_DEFRAMER_H
#define _MODEMSTUFF_AX25_DEFRAMER_H

#include <modemstuff/modemstuff.h>
#include <modemstuff/ax25_frame.h>

#define MAX_REPEATER_COUNT 8
#define MAX_INFO_LEN 255
#define FLAG 0x7e
#define MIN_FRAME_LEN 18
#define MAX_FRAME_LEN (7 + 7 + 7 * MAX_REPEATER_COUNT + 2 + MAX_INFO_LEN + 2)
// 7 bytes for destination address field
// 7 bytes for source address field
// 7 bytes for a maximum of 8 path address fields
// 2 bytes for control and protocol fields
// 255 bytes for info field
// 2 bytes for FCS

typedef struct ms_ax25_deframer
{
    ms_byte buffer[MAX_FRAME_LEN];
    int buffer_pos;

    ms_byte raw_bits;
    ms_byte unstuffed_bits;
    ms_byte unstuffed_bit_count;
    ms_byte ones_count;

} ms_ax25_deframer_t;

/**
 * Initializes an ms_ax25_deframer_t structure.

 * @param deframer Pointer to the ms_ax25_deframer_t structure to be initialized.
 */
void ms_ax25_deframer_init(ms_ax25_deframer_t *deframer);

/**
 * Processes a single bit of data in the deframer.

 * @param deframer Pointer to the ms_ax25_deframer_t structure to be processed.
 * @param frame Pointer to the ms_ax25_frame_t structure where the frame will be stored.
 * @param bit The bit to be processed.
 *
 * @return 1 if a frame is ready to be read, 0 otherwise.
 */
ms_bit ms_ax25_deframer_process(ms_ax25_deframer_t *deframer, ms_ax25_frame_t *frame, ms_bit bit);

#endif // _MODEMSTUFF_AX25DEFRAMER_H
