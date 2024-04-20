#ifndef _AX25TNC_MODULATOR_H
#define _AX25TNC_MODULATOR_H

#include "config.h"

#include <modemstuff/fsk.h>
#include <modemstuff/linecode.h>
#include <hamstuff/ax25_frame.h>

typedef struct ax25tnc_modulator
{
    ms_fsk_generator_t ms_fsk_generator;
    ms_float baud_rate;
    ms_float sample_rate;
    int tx_delay; // in milliseconds
    int tx_tail;  // in milliseconds

    ms_linecode_nrzi_encoder_t nrzi;

    void (*samples_callback)(ms_float *samples, int samples_count);

} ax25tnc_modulator_t;

/**
 * Initialize the modulator.
 *
 * @param mod The modulator to initialize.
 * @param config The configuration to use.
 */
void ax25tnc_modulator_init(
    ax25tnc_modulator_t *mod,
    ax25tnc_config_t *config);

/**
 * Set the callbacks for the modulator.
 *
 * @param mod The modulator.
 * @param samples_callback The callback for modulated samples.
 */
void ax25tnc_modulator_set_callbacks(
    ax25tnc_modulator_t *mod,
    void (*samples_callback)(ms_float *samples, int samples_count));

/**
 * Modulate an AX.25 frame.
 *
 * @param mod The modulator.
 * @param frame The frame to process.
 */
void ax25tnc_modulator_process(
    ax25tnc_modulator_t *mod,
    hs_ax25_frame_t *frame);

/**
 * Destroy the modulator.
 *
 * @param mod The modulator to destroy.
 */
void ax25tnc_modulator_destroy(
    ax25tnc_modulator_t *mod);

#endif // _AX25TNC_MODULATOR_H