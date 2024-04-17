#ifndef _AUDMOD_DEMODULATOR_H
#define _AUDMOD_DEMODULATOR_H

#include <modemstuff/fsk.h>
#include <modemstuff/bitdet.h>
#include <modemstuff/linecode.h>
#include <hamstuff/ax25_deframer.h>

typedef struct audmod_demodulator
{
    ms_fsk_detector_t fsk_detector;
    ms_bit_detector_t bit_detector;
    ms_linecode_nrzi_decoder_t nrzi_decoder;
    hs_ax25_deframer_t ax25_deframer;
    void (*frame_callback)(hs_ax25_frame_t *frame);

} audmod_demodulator_t;

int audmod_demodulator_init(
    audmod_demodulator_t *demod,
    ms_float mark_freq,
    ms_float space_freq,
    ms_float baud_rate,
    ms_float sample_rate);

void audmod_demodulator_set_callbacks(
    audmod_demodulator_t *demod,
    void (*frame_callback)(hs_ax25_frame_t *frame));

void audmod_demodulator_process(
    audmod_demodulator_t *demod,
    ms_float *samples,
    int samples_count);

void audmod_demodulator_destroy(
    audmod_demodulator_t *demod);
    
#endif // _AUDMOD_DEMODULATOR_H