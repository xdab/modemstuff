#include <modemstuff/bitdet.h>

void ms_bit_detector_init(ms_bit_detector_t *bitdec, ms_float sample_rate, ms_float bit_rate)
{
    bitdec->bit_period = sample_rate / bit_rate;
    bitdec->samples_to_return = 0;
    bitdec->last_bit = MS_BIT_NONE;
}

ms_bit ms_bit_detector_process(ms_bit_detector_t *bitdec, ms_float soft_symbol)
{
    ms_bit bit;

    if (soft_symbol > 0.0)
        bit = MS_BIT_ONE;
    else if (soft_symbol <= 0.0)
        bit = MS_BIT_ZERO;

    if (bit != bitdec->last_bit)
    {
        bitdec->last_bit = bit;
        bitdec->samples_to_return = bitdec->bit_period / 2;
        return MS_BIT_NONE;
    }

    if (bitdec->samples_to_return == 0)
    {
        bitdec->samples_to_return = bitdec->bit_period;
        return bit;
    }

    bitdec->samples_to_return--;    
    return MS_BIT_NONE;
}
