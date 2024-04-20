#ifndef _MODEMSTUFF_LINECODE_H
#define _MODEMSTUFF_LINECODE_H

#include <modemstuff/modemstuff.h>

typedef struct ms_linecode_nrzi_decoder
{
    ms_bit last_bit;
} ms_linecode_nrzi_decoder_t;

/**
 * Initialize the NRZI decoder.
 *
 * @param decoder The decoder to initialize.
 */
void ms_linecode_nrzi_decoder_init(ms_linecode_nrzi_decoder_t *decoder);

/**
 * Decode a bit using NRZI.
 *
 * @param decoder The decoder.
 * @param bit The bit to decode.
 *
 * @return The decoded bit.
 */
ms_bit ms_linecode_nrzi_decode(ms_linecode_nrzi_decoder_t *decoder, ms_bit bit);

typedef struct ms_linecode_nrzi_encoder
{
    ms_bit nrzi_bit;
} ms_linecode_nrzi_encoder_t;

/**
 * Initialize the NRZI encoder.
 *
 * @param encoder The encoder to initialize.
 */
void ms_linecode_nrzi_encoder_init(ms_linecode_nrzi_encoder_t *encoder);

/**
 * Encode a bit using NRZI.
 *
 * @param encoder The encoder.
 * @param bit The bit to encode.
 *
 * @return The encoded bit.
 */
ms_bit ms_linecode_nrzi_encode(ms_linecode_nrzi_encoder_t *encoder, ms_bit bit);

#endif // _MODEMSTUFF_LINECODE_H
