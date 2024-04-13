#include <modemstuff/linecode.h>

void ms_linecode_nrzi_decoder_init(ms_linecode_nrzi_decoder_t *decoder)
{
    decoder->last_bit = MS_BIT_ZERO;
}

ms_bit ms_linecode_nrzi_decode(ms_linecode_nrzi_decoder_t *decoder, ms_bit bit)
{
    ms_bit output_bit;

    if (bit == MS_BIT_NONE)
        return MS_BIT_NONE;
        
    output_bit = (bit == decoder->last_bit) ? MS_BIT_ONE : MS_BIT_ZERO;
    decoder->last_bit = bit;

    return output_bit;
}
