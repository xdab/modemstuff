#ifndef _HAMSTUFF_AX25_FRAMER_H
#define _HAMSTUFF_AX25_FRAMER_H

#include <hamstuff/hamstuff.h>
#include <hamstuff/ax25/ax25.h>
#include <hamstuff/ax25/packet.h>

typedef unsigned int _bb_unit_t;

#define HS_AX25_FRAMER_BIT_BUFFER_SIZE 256
#define HS_AX25_FRAMER_BIT_BUFFER_UNIT (sizeof(unsigned int) * 8)

#define HS_AX25_FRAMER_MIN_HEAD_FLAGS 4
#define HS_AX25_FRAMER_MIN_TAIL_FLAGS 2


typedef struct hs_ax25_framer
{
    int head_flags;
    int tail_flags;

    _bb_unit_t bit_buffer[HS_AX25_FRAMER_BIT_BUFFER_SIZE];
    int _bit_buffer_unit;
    int _bit_buffer_bit;
    int _bit_buffer_total_bits;
    int _reading_bit;
} hs_ax25_framer_t;

/**
 * Initialize an AX.25 framer.
 *
 * @param framer The framer to initialize.
 */
void hs_ax25_framer_init(hs_ax25_framer_t *framer);

/**
 * Frame an AX.25 packet.
 *
 * @param framer The framer.
 * @param packet The packet to frame.
 * 
 * @return The number of bits in the frame.
 */
int hs_ax25_framer_process(hs_ax25_framer_t *framer, hs_ax25_packet_t *packet);

/**
 * Get the next bit from the framer.
 *
 * @param framer The framer.
 * 
 * @return The next bit or -1 if there are no more bits.
 */
hs_bit hs_ax25_framer_next_bit(hs_ax25_framer_t *framer);

#endif // _HAMSTUFF_AX25_FRAMER_H
