#ifndef _AUDMOD_DEMODULATOR_H
#define _AUDMOD_DEMODULATOR_H

#include "config.h"

#include <modemstuff/fsk.h>
#include <modemstuff/bitdet.h>
#include <modemstuff/linecode.h>
#include <hamstuff/ax25_deframer.h>

typedef struct audmod_demodulator
{
    ms_fsk_detector_t fsk_detector;
    ms_bit_detector_t bit_detector;
    ms_linecode_nrzi_decoder_t nrzi_decoder;
    hs_ax25_deframer_t ax25_deframer;
    void (*frame_callback)(hs_ax25_frame_t *frame);

} audmod_demodulator_t;

/**
 * Initialize the demodulator.
 *
 * @param demod The demodulator to initialize.
 * @param config The configuration to use.
 * @return 0 on success, 1 on error.
 */
int audmod_demodulator_init(
    audmod_demodulator_t *demod,
    audmod_config_t *config);

/**
 * Set the callbacks for the demodulator.
 *
 * @param demod The demodulator.
 * @param frame_callback The callback for demodulated frames.
 */
void audmod_demodulator_set_callbacks(
    audmod_demodulator_t *demod,
    void (*frame_callback)(hs_ax25_frame_t *frame));

/**
 * Process samples.
 *
 * @param demod The demodulator.
 * @param samples The samples to process.
 * @param samples_count The number of samples.
 */
void audmod_demodulator_process(
    audmod_demodulator_t *demod,
    ms_float *samples,
    int samples_count);

/**
 * Destroy the demodulator.
 *
 * @param demod The demodulator to destroy.
 */
void audmod_demodulator_destroy(
    audmod_demodulator_t *demod);

#endif // _AUDMOD_DEMODULATOR_H