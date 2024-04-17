#include "modulator.h"

#include <stdlib.h>

void _audmod_modulator_process(audmod_modulator_t *mod, ms_bit bit)
{
    ms_float samples[512]; // TODO make variable
    int samples_count;

    samples_count = ms_fsk_generator_process(&mod->ms_fsk_generator, bit, samples, 512);

    if (mod->samples_callback != NULL)
        mod->samples_callback(samples, samples_count);
}

void audmod_modulator_init(audmod_modulator_t *mod, ms_float mark_freq, ms_float space_freq, ms_float baud_rate, ms_float sample_rate)
{
    ms_fsk_generator_init(&mod->ms_fsk_generator, mark_freq, space_freq, baud_rate, sample_rate);
    mod->baud_rate = baud_rate;
    mod->sample_rate = sample_rate;

    // TODO make variable
    mod->tx_delay = 400; // 400 ms
    mod->tx_tail = 50;   // 50 ms
}

void audmod_modulator_set_callbacks(audmod_modulator_t *mod, void (*samples_callback)(ms_float *samples, int samples_count))
{
    mod->samples_callback = samples_callback;
}

void audmod_modulator_process(audmod_modulator_t *mod, hs_ax25_frame_t *frame)
{
    // Make bit array
    // Insert TX delay worth of flags (without stuffing)
    // Insert packed frame (with stuffing)
    // Insert TX tail worth of flags (without stuffing)
    // Send 
}
