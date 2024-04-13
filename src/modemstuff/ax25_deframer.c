#include <modemstuff/ax25_deframer.h>
#include <modemstuff/ax25_crc.h>

#include <stdio.h> // debug fprintf

void ms_ax25_deframer_init(ms_ax25_deframer_t *deframer)
{
    deframer->buffer_pos = 0;
    deframer->raw_bits = 0;
    deframer->unstuffed_bits = 0;
    deframer->unstuffed_bit_count = 0;
}

ms_bit ms_ax25_deframer_process(ms_ax25_deframer_t *deframer, ms_ax25_frame_t *frame, ms_bit bit)
{
    int i;
    ms_bit ret = 0;

    // Shift in the new bit
    deframer->raw_bits = (deframer->raw_bits << 1) | bit;

    // Flag detection
    if (deframer->raw_bits == FLAG)
    {
        // Look at the buffer to see if there is a valid frame
        if (deframer->buffer_pos >= MIN_FRAME_LEN)
        {
            // Validate FCS
            unsigned short received_fcs = deframer->buffer[deframer->buffer_pos - 2] | (deframer->buffer[deframer->buffer_pos - 1] << 8);

            unsigned short computed_fcs = ms_ax25_crc16_init();
            for (i = 0; i < deframer->buffer_pos - 2; i++)
                computed_fcs = ms_ax25_crc16_update(deframer->buffer[i], computed_fcs);
            computed_fcs = ms_ax25_crc16_finalize(computed_fcs);

            if (computed_fcs == received_fcs)
            {
                // Unpack the frame and return 1 to signal that a frame is ready
                ms_ax25_frame_unpack(frame, deframer->buffer, deframer->buffer_pos);
                ret = 1;
            }
        }

        // Reset the buffer
        deframer->buffer_pos = 0;
        deframer->raw_bits = 0;
        deframer->unstuffed_bits = 0;
        deframer->unstuffed_bit_count = 0;
        deframer->ones_count = 0;
    }

    else // Not a flag, actually process the bit
    {
        // Unstuffing
        if (bit == 0)
        {
            if (deframer->ones_count < 5)
            {
                deframer->unstuffed_bits = (deframer->unstuffed_bits >> 1) | (bit << 7);
                deframer->unstuffed_bit_count++;
            }
            deframer->ones_count = 0; // Reset the ones count
        }
        else // (bit == 1)
        {
            deframer->unstuffed_bits = (deframer->unstuffed_bits >> 1) | (bit << 7);
            deframer->ones_count++;
            deframer->unstuffed_bit_count++;
        }

        // If we have a full byte, store it in the buffer
        if (deframer->unstuffed_bit_count == 8)
        {
            // Check for buffer overflow
            if (deframer->buffer_pos >= 255)
            {
                deframer->buffer_pos = 0;
            }

            deframer->buffer[deframer->buffer_pos++] = deframer->unstuffed_bits;
            deframer->unstuffed_bit_count = 0;
        }
    }

    // Return 1 if a frame is ready
    return ret;
}
