#include "demodulator.h"

#include <stdio.h>
#include <stdlib.h>

#define NOISE_AMPLITUDE 0.02
#define NOISE_RESOLUTION 10000
#define NOISE() (NOISE_AMPLITUDE * (2.0 * (((ms_float) (rand() % NOISE_RESOLUTION)) / NOISE_RESOLUTION) - 1.0))
FILE *f;

int audmod_demodulator_init(audmod_demodulator_t *demod, ms_float mark_freq, ms_float space_freq, ms_float baud_rate, ms_float sample_rate)
{
    if (ms_fsk_detector_init(&demod->fsk_detector, mark_freq, space_freq, baud_rate, sample_rate))
    {
        fprintf(stderr, "Error: ms_fsk_detector_init() failed\n");
        return -1;
    }

    ms_bit_detector_init(&demod->bit_detector, sample_rate, baud_rate);
    ms_linecode_nrzi_decoder_init(&demod->nrzi_decoder);
    hs_ax25_deframer_init(&demod->ax25_deframer);

    f = fopen("output.raw", "wb");

    return 0;
}

void audmod_demodulator_set_callbacks(audmod_demodulator_t *demod, void (*frame_callback)(hs_ax25_frame_t *frame))
{
    demod->frame_callback = frame_callback;
}

void audmod_demodulator_process(audmod_demodulator_t *demod, ms_float *samples, int samples_count)
{
    int i;
    ms_float symbol;
    ms_bit bit;
    hs_ax25_frame_t frame;

    for (i = 0; i < samples_count; i++)
    {
        samples[i] += NOISE();
        symbol = ms_fsk_detector_process(&demod->fsk_detector, samples[i]);
        fwrite(&symbol, sizeof(ms_float), 1, f);
        
        bit = ms_bit_detector_process(&demod->bit_detector, symbol);
        bit = ms_linecode_nrzi_decode(&demod->nrzi_decoder, bit);

        if ((bit != MS_BIT_NONE) && hs_ax25_deframer_process(&demod->ax25_deframer, &frame, bit) && (demod->frame_callback != NULL))
            demod->frame_callback(&frame);
    }
}

void audmod_demodulator_destroy(audmod_demodulator_t *demod)
{
    ms_fsk_detector_destroy(&demod->fsk_detector);
}
