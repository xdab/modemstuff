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

void ms_bit_detector_init(ms_bit_detector_t *bitdec, ms_float sample_rate, ms_float bit_rate);

ms_bit ms_bit_detector_process(ms_bit_detector_t *bitdec, ms_float soft_symbol);

#endif // _MODEMSTUFF_BITDEC_H