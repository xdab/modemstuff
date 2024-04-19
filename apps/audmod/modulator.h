#ifndef _AUDMOD_MODULATOR_H
#define _AUDMOD_MODULATOR_H

#include <modemstuff/fsk.h>
#include <hamstuff/ax25_frame.h>

typedef struct audmod_modulator
{
    ms_fsk_generator_t ms_fsk_generator;
    ms_float baud_rate;
    ms_float sample_rate;
    int tx_delay; // in milliseconds
    int tx_tail; // in milliseconds

    ms_bit last_bit;
    ms_bit nrzi_bit;

    void (*samples_callback)(ms_float *samples, int samples_count);

} audmod_modulator_t;

void audmod_modulator_init(
    audmod_modulator_t *mod,
    ms_float mark_freq,
    ms_float space_freq,
    ms_float baud_rate,
    ms_float sample_rate);

void audmod_modulator_set_callbacks(
    audmod_modulator_t *mod,
    void (*samples_callback)(ms_float *samples, int samples_count));

void audmod_modulator_process(
    audmod_modulator_t *mod,
    hs_ax25_frame_t *frame);

void audmod_modulator_destroy(
    audmod_modulator_t *mod);
    
#endif // _AUDMOD_MODULATOR_H