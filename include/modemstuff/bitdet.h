#ifndef _MODEMSTUFF_BITDEC_H
#define _MODEMSTUFF_BITDEC_H

#include <modemstuff/modemstuff.h>
#include <modemstuff/dds.h>

typedef struct
{
    int bit_period;
    int samples_to_return;
    ms_bit last_bit;
} ms_bit_detector_t;

/**
 * Initialize the bit detector.
 *
 * @param bitdec The bit detector to initialize.
 * @param sample_rate The sample rate.
 * @param bit_rate Expected bit rate.
 */
void ms_bit_detector_init(ms_bit_detector_t *bitdec, ms_float sample_rate, ms_float bit_rate);

/**
 * Process a soft symbol.
 *
 * @param bitdec The bit detector.
 * @param soft_symbol The soft symbol to process.
 *
 * @return The detected bit or MS_BIT_NONE if not enough samples have been processed.
 */
ms_bit ms_bit_detector_process(ms_bit_detector_t *bitdec, ms_float soft_symbol);

#endif // _MODEMSTUFF_BITDEC_H