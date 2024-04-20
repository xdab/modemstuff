#include "demodulator.h"

int ax25tnc_demodulator_init(ax25tnc_demodulator_t *demod, ax25tnc_config_t *config)
{
    if (ms_fsk_detector_init(&demod->fsk_detector, config->mark_freq, config->space_freq, config->baud_rate, config->sample_rate))
        return 1;

    ms_bit_detector_init(&demod->bit_detector, config->sample_rate, config->baud_rate);
    ms_linecode_nrzi_decoder_init(&demod->nrzi_decoder);
    hs_ax25_deframer_init(&demod->ax25_deframer);

    return 0;
}

void ax25tnc_demodulator_set_callbacks(ax25tnc_demodulator_t *demod, void (*frame_callback)(hs_ax25_frame_t *frame))
{
    demod->frame_callback = frame_callback;
}

void ax25tnc_demodulator_process(ax25tnc_demodulator_t *demod, ms_float *samples, int samples_count)
{
    int i;
    ms_float symbol;
    ms_bit bit;
    hs_ax25_frame_t frame;

    for (i = 0; i < samples_count; i++)
    {
        symbol = ms_fsk_detector_process(&demod->fsk_detector, samples[i]);
        bit = ms_bit_detector_process(&demod->bit_detector, symbol);
        bit = ms_linecode_nrzi_decode(&demod->nrzi_decoder, bit);

        if ((bit != MS_BIT_NONE) && hs_ax25_deframer_process(&demod->ax25_deframer, &frame, bit) && demod->frame_callback)
            demod->frame_callback(&frame);
    }
}

void ax25tnc_demodulator_destroy(ax25tnc_demodulator_t *demod)
{
    ms_fsk_detector_destroy(&demod->fsk_detector);
}
