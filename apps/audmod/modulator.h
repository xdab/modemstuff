#ifndef _AUDMOD_MODULATOR_H
#define _AUDMOD_MODULATOR_H

#include "config.h"

#include <modemstuff/fsk.h>
#include <modemstuff/linecode.h>
#include <hamstuff/ax25_frame.h>

typedef struct audmod_modulator
{
    ms_fsk_generator_t ms_fsk_generator;
    ms_float baud_rate;
    ms_float sample_rate;
    int tx_delay; // in milliseconds
    int tx_tail;  // in milliseconds

    ms_linecode_nrzi_encoder_t nrzi;

    void (*samples_callback)(ms_float *samples, int samples_count);

} audmod_modulator_t;

/**
 * Initialize the modulator.
 *
 * @param mod The modulator to initialize.
 * @param config The configuration to use.
 */
void audmod_modulator_init(
    audmod_modulator_t *mod,
    audmod_config_t *config);

/**
 * Set the callbacks for the modulator.
 *
 * @param mod The modulator.
 * @param samples_callback The callback for modulated samples.
 */
void audmod_modulator_set_callbacks(
    audmod_modulator_t *mod,
    void (*samples_callback)(ms_float *samples, int samples_count));

/**
 * Modulate an AX.25 frame.
 *
 * @param mod The modulator.
 * @param frame The frame to process.
 */
void audmod_modulator_process(
    audmod_modulator_t *mod,
    hs_ax25_frame_t *frame);

/**
 * Destroy the modulator.
 *
 * @param mod The modulator to destroy.
 */
void audmod_modulator_destroy(
    audmod_modulator_t *mod);

#endif // _AUDMOD_MODULATOR_H