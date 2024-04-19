#include "modulator.h"

#include <stdio.h>
#include <stdlib.h>

#define MIN_HEAD_FLAGS 4
#define MIN_TAIL_FLAGS 2
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void _audmod_modulator_process(audmod_modulator_t *mod, ms_bit bit)
{
    ms_float samples[512]; // TODO make variable
    int samples_count;

    // NRZI
    if (bit == MS_BIT_ZERO)
        mod->nrzi_bit ^= 1;

    samples_count = ms_fsk_generator_process(&mod->ms_fsk_generator, mod->nrzi_bit, samples, 512);

    if (mod->samples_callback != NULL)
        mod->samples_callback(samples, samples_count);
}

void audmod_modulator_init(audmod_modulator_t *mod, ms_float mark_freq, ms_float space_freq, ms_float baud_rate, ms_float sample_rate)
{
    ms_fsk_generator_init(&mod->ms_fsk_generator, mark_freq, space_freq, baud_rate, sample_rate);

    mod->baud_rate = baud_rate;
    mod->sample_rate = sample_rate;

    // TODO make variable
    mod->tx_delay = 200;
    mod->tx_tail = 20;
}

void audmod_modulator_set_callbacks(audmod_modulator_t *mod, void (*samples_callback)(ms_float *samples, int samples_count))
{
    mod->samples_callback = samples_callback;
}

void audmod_modulator_process(audmod_modulator_t *mod, hs_ax25_frame_t *frame)
{
    char packed_frame[512];

    mod->last_bit = MS_BIT_ZERO;
    mod->nrzi_bit = MS_BIT_ZERO;

    // Insert TX delay worth of flags (without stuffing)
    int flag_count = mod->tx_delay * mod->baud_rate / 8000;
    flag_count = MAX(flag_count, MIN_HEAD_FLAGS);
    for (int i = 0; i < flag_count; i++)
    {
        _audmod_modulator_process(mod, 0);
        for (int j = 0; j < 6; j++)
            _audmod_modulator_process(mod, 1);
        _audmod_modulator_process(mod, 0);
    }

    // Insert packed frame (with stuffing)
    int packed_frame_bytes = hs_ax25_frame_pack(frame, packed_frame);
    int ones_count = 0;
    for (int i = 0; i < packed_frame_bytes; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int bit = packed_frame[i] & (1 << j);
            bit = bit ? 1 : 0;

            _audmod_modulator_process(mod, bit);

            if (bit)
                ones_count++;
            else
                ones_count = 0;

            if (ones_count == 5)
            {
                _audmod_modulator_process(mod, 0);
                ones_count = 0;
            }
        }
    }

    // Insert TX tail worth of flags (without stuffing)
    flag_count = mod->tx_tail * mod->baud_rate / 8000;
    flag_count = MAX(flag_count, MIN_TAIL_FLAGS);
    for (int i = 0; i < flag_count; i++)
    {
        _audmod_modulator_process(mod, 0);
        for (int j = 0; j < 6; j++)
            _audmod_modulator_process(mod, 1);
        _audmod_modulator_process(mod, 0);
    }
}

void audmod_modulator_destroy(audmod_modulator_t *mod)
{
    ms_fsk_generator_destroy(&mod->ms_fsk_generator);
}