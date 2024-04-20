#include "modulator.h"

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_TX_DELAY 300
#define DEFAULT_TX_TAIL 50

#define MIN_HEAD_FLAGS 4
#define MIN_TAIL_FLAGS 2
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void _ax25tnc_modulator_process(ax25tnc_modulator_t *mod, ms_bit bit)
{
    const int MAX_SAMPLES = 512; // TODO make variable

    ms_float samples[MAX_SAMPLES];
    int samples_count;

    bit = ms_linecode_nrzi_encode(&mod->nrzi, bit);
    samples_count = ms_fsk_generator_process(&mod->ms_fsk_generator, bit, samples, MAX_SAMPLES);

    if (mod->samples_callback != NULL)
        mod->samples_callback(samples, samples_count);
}

void ax25tnc_modulator_init(ax25tnc_modulator_t *mod, ax25tnc_config_t *config)
{
    ms_fsk_generator_init(&mod->ms_fsk_generator, config->mark_freq, config->space_freq, config->baud_rate, config->sample_rate);
    mod->baud_rate = config->baud_rate;
    mod->sample_rate = config->sample_rate;
    mod->tx_delay = DEFAULT_TX_DELAY;
    mod->tx_tail = DEFAULT_TX_TAIL;
}

void ax25tnc_modulator_set_callbacks(ax25tnc_modulator_t *mod, void (*samples_callback)(ms_float *samples, int samples_count))
{
    mod->samples_callback = samples_callback;
}

void ax25tnc_modulator_process(ax25tnc_modulator_t *mod, hs_ax25_frame_t *frame)
{
    const int MAX_PACKED_FRAME_SIZE = 512;

    char packed_frame[MAX_PACKED_FRAME_SIZE];

    ms_linecode_nrzi_encoder_init(&mod->nrzi);

    // Insert TX delay worth of flags (without stuffing)
    int flag_count = mod->tx_delay * mod->baud_rate / 8000;
    flag_count = MAX(flag_count, MIN_HEAD_FLAGS);
    for (int i = 0; i < flag_count; i++)
    {
        _ax25tnc_modulator_process(mod, 0);
        for (int j = 0; j < 6; j++)
            _ax25tnc_modulator_process(mod, 1);
        _ax25tnc_modulator_process(mod, 0);
    }

    // Insert packed frame (with stuffing)
    int packed_frame_bytes = hs_ax25_frame_pack(frame, packed_frame);
    int ones_count = 0;
    for (int i = 0; i < packed_frame_bytes; i++)
        for (int j = 0; j < 8; j++)
        {
            int bit = packed_frame[i] & (1 << j);
            bit = bit ? 1 : 0;

            _ax25tnc_modulator_process(mod, bit);

            if (bit)
                ones_count++;
            else
                ones_count = 0;

            if (ones_count == 5)
            {
                _ax25tnc_modulator_process(mod, 0);
                ones_count = 0;
            }
        }

    // Insert TX tail worth of flags (without stuffing)
    flag_count = mod->tx_tail * mod->baud_rate / 8000;
    flag_count = MAX(flag_count, MIN_TAIL_FLAGS);
    for (int i = 0; i < flag_count; i++)
    {
        _ax25tnc_modulator_process(mod, 0);
        for (int j = 0; j < 6; j++)
            _ax25tnc_modulator_process(mod, 1);
        _ax25tnc_modulator_process(mod, 0);
    }
}

void ax25tnc_modulator_destroy(ax25tnc_modulator_t *mod)
{
    ms_fsk_generator_destroy(&mod->ms_fsk_generator);
}