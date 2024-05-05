#include "modulator.h"

#include <modemstuff/linecode.h>
#include <hamstuff/ax25/framer.h>

#define DEFAULT_TX_DELAY 320
#define DEFAULT_TX_TAIL 80

#define MIN_HEAD_FLAGS 4
#define MIN_TAIL_FLAGS 2
#define MAX(a, b) ((a) > (b) ? (a) : (b))

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

void ax25tnc_modulator_process(ax25tnc_modulator_t *mod, hs_ax25_packet_t *packet)
{
    const int MAX_SAMPLES = 512; // TODO variable

    hs_ax25_framer_t framer;
    ms_linecode_nrzi_encoder_t nrzi;
    hs_bit bit;
    int frame_bits;
    int bit_idx;
    int unit_idx;

    ms_float samples[MAX_SAMPLES];
    int samples_count;

    hs_ax25_framer_init(&framer);
    framer.head_flags = MAX(mod->tx_delay * mod->baud_rate / 8000, MIN_HEAD_FLAGS);
    framer.tail_flags = MAX(mod->tx_tail * mod->baud_rate / 8000, MIN_TAIL_FLAGS);

    ms_linecode_nrzi_encoder_init(&nrzi);
    (void)hs_ax25_framer_process(&framer, packet);

    while ((bit = hs_ax25_framer_next_bit(&framer)) != -1)
    {
        // Use NRZI line code
        bit = ms_linecode_nrzi_encode(&nrzi, bit);

        // Generate and send FSK samples
        samples_count = ms_fsk_generator_process(&mod->ms_fsk_generator, bit, samples, MAX_SAMPLES);
        if (mod->samples_callback)
            mod->samples_callback(samples, samples_count);
    }
}

void ax25tnc_modulator_destroy(ax25tnc_modulator_t *mod)
{
    ms_fsk_generator_destroy(&mod->ms_fsk_generator);
}