#include <hamstuff/ax25_framer.h>

void _hs_ax25_insert_bit(hs_ax25_framer_t *framer, hs_bit bit)
{
    if (framer->_bit_buffer_unit >= HS_AX25_FRAMER_BIT_BUFFER_SIZE)
        return; // Buffer overflow

    framer->bit_buffer[framer->_bit_buffer_unit] |= (bit << framer->_bit_buffer_bit);
    framer->_bit_buffer_bit++;
    framer->_bit_buffer_total_bits++;

    if (framer->_bit_buffer_bit >= HS_AX25_FRAMER_BIT_BUFFER_UNIT)
    {
        framer->_bit_buffer_unit++;
        framer->_bit_buffer_bit = 0;
    }
}

void _hs_ax25_insert_byte(hs_ax25_framer_t *framer, hs_byte byte)
{
    int i;

    for (i = 0; i < 8; i++)
        _hs_ax25_insert_bit(framer, (byte & (1 << i)) ? 1 : 0);
}

void hs_ax25_framer_init(hs_ax25_framer_t *framer)
{
    framer->head_flags = HS_AX25_FRAMER_MIN_HEAD_FLAGS;
    framer->tail_flags = HS_AX25_FRAMER_MIN_TAIL_FLAGS;
    framer->_bit_buffer_unit = 0;
    framer->_bit_buffer_bit = 0;
    framer->_bit_buffer_total_bits = 0;
    framer->_reading_bit = 0;
}

int hs_ax25_framer_process(hs_ax25_framer_t *framer, hs_ax25_packet_t *packet)
{
    const int MAX_PACKED_PACKET_SIZE = 512;

    char packed_packet[MAX_PACKED_PACKET_SIZE];
    int packed_packet_bytes;
    int i, j;
    int ones_count;
    hs_bit bit;

    // Reset bit buffer
    framer->_bit_buffer_unit = 0;
    framer->_bit_buffer_bit = 0;
    framer->_bit_buffer_total_bits = 0;

    // Insert head flags without stuffing
    for (i = 0; i < framer->head_flags; i++)
        _hs_ax25_insert_byte(framer, HS_AX25_FLAG);

    // Pack packet
    packed_packet_bytes = hs_ax25_packet_pack(packet, packed_packet);

    // Insert packed packet with bit stuffing
    ones_count = 0;
    for (i = 0; i < packed_packet_bytes; i++)
        for (j = 0; j < 8; j++)
        {
            bit = (packed_packet[i] & (1 << j)) ? 1 : 0;

            _hs_ax25_insert_bit(framer, bit);

            if (bit)
                ones_count++;
            else
                ones_count = 0;

            if (ones_count == 5)
            {
                _hs_ax25_insert_bit(framer, 0);
                ones_count = 0;
            }
        }

    // Insert tail flags without stuffing
    for (i = 0; i < framer->tail_flags; i++)
        _hs_ax25_insert_byte(framer, HS_AX25_FLAG);

    framer->_reading_bit = 0;
    return framer->_bit_buffer_total_bits;
}

hs_bit hs_ax25_framer_next_bit(hs_ax25_framer_t *framer)
{
    int unit_idx;
    int bit_idx;

    if (framer->_reading_bit >= framer->_bit_buffer_total_bits)
        return -1;

    unit_idx = framer->_reading_bit / HS_AX25_FRAMER_BIT_BUFFER_UNIT;
    bit_idx = framer->_reading_bit % HS_AX25_FRAMER_BIT_BUFFER_UNIT;
    framer->_reading_bit++;

    return (framer->bit_buffer[unit_idx] >> bit_idx) & 1;
}