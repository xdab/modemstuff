#ifndef _HAMSTUFF_AX25_DEFRAMER_H
#define _HAMSTUFF_AX25_DEFRAMER_H

#include <hamstuff/hamstuff.h>
#include <hamstuff/ax25_frame.h>

typedef char hs_bit;

#define HS_AX25_MAX_REPEATER_COUNT 8
#define HS_AX25_MAX_INFO_LEN 255
#define HS_AX25_FLAG 0x7e
#define HS_AX25_MIN_FRAME_LEN 18
#define HS_AX25_MAX_FRAME_LEN (7 + 7 + 7 * HS_AX25_MAX_REPEATER_COUNT + 2 + HS_AX25_MAX_INFO_LEN + 2)
// 7 bytes for destination address field
// 7 bytes for source address field
// 7 bytes for a maximum of 8 path address fields
// 2 bytes for control and protocol fields
// 255 bytes for info field
// 2 bytes for FCS

typedef struct hs_ax25_deframer
{
    hs_byte buffer[HS_AX25_MAX_FRAME_LEN];
    int buffer_pos;

    hs_byte raw_bits;
    hs_byte unstuffed_bits;
    hs_byte unstuffed_bit_count;
    hs_byte ones_count;

} hs_ax25_deframer_t;

/**
 * Initializes an hs_ax25_deframer_t structure.

 * @param deframer Pointer to the hs_ax25_deframer_t structure to be initialized.
 */
void hs_ax25_deframer_init(hs_ax25_deframer_t *deframer);

/**
 * Processes a single bit of data in the deframer.

 * @param deframer Pointer to the hs_ax25_deframer_t structure to be processed.
 * @param frame Pointer to the hs_ax25_frame_t structure where the frame will be stored.
 * @param bit The bit to be processed.
 *
 * @return 1 if a frame is ready to be read, 0 otherwise.
 */
hs_bit hs_ax25_deframer_process(hs_ax25_deframer_t *deframer, hs_ax25_frame_t *frame, hs_bit bit);

#endif // _HAMSTUFF_AX25_DEFRAMER_H
