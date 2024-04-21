#include "demodulator.h"

int test_demod_demodulator_init(test_demod_demodulator_t *demod, test_demod_config_t *config)
{
    if (ms_fsk_detector_init(&demod->fsk_detector, config->mark_freq, config->space_freq, config->baud_rate, config->sample_rate))
        return 1;

    ms_bit_detector_init(&demod->bit_detector, config->sample_rate, config->baud_rate);
    ms_linecode_nrzi_decoder_init(&demod->nrzi_decoder);
    hs_ax25_deframer_init(&demod->ax25_deframer);

    return 0;
}

void test_demod_demodulator_set_callbacks(test_demod_demodulator_t *demod, void (*packet_callback)(hs_ax25_packet_t *packet))
{
    demod->packet_callback = packet_callback;
}

void test_demod_demodulator_process(test_demod_demodulator_t *demod, ms_float *samples, int samples_count)
{
    int i;
    ms_float symbol;
    ms_bit bit;
    hs_ax25_packet_t packet;

    for (i = 0; i < samples_count; i++)
    {
        symbol = ms_fsk_detector_process(&demod->fsk_detector, samples[i]);
        bit = ms_bit_detector_process(&demod->bit_detector, symbol);
        bit = ms_linecode_nrzi_decode(&demod->nrzi_decoder, bit);

        if ((bit != MS_BIT_NONE) && hs_ax25_deframer_process(&demod->ax25_deframer, &packet, bit) && demod->packet_callback)
            demod->packet_callback(&packet);
    }
}

void test_demod_demodulator_destroy(test_demod_demodulator_t *demod)
{
    ms_fsk_detector_destroy(&demod->fsk_detector);
}
