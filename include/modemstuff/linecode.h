#ifndef _MODEMSTUFF_LINECODE_H
#define _MODEMSTUFF_LINECODE_H 

#include <modemstuff/modemstuff.h>

typedef struct ms_linecode_nrzi
{
    ms_bit last_bit;
} ms_linecode_nrzi_decoder_t;

void ms_linecode_nrzi_decoder_init(ms_linecode_nrzi_decoder_t *decoder);
ms_bit ms_linecode_nrzi_decode(ms_linecode_nrzi_decoder_t *decoder, ms_bit bit);

#endif // _MODEMSTUFF_LINECODE_H

