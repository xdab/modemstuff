#ifndef _AX25TNC_DEMODULATOR_H
#define _AX25TNC_DEMODULATOR_H

#include "config.h"

#include <modemstuff/fsk.h>
#include <modemstuff/bitdet.h>
#include <modemstuff/linecode.h>

#include <hamstuff/ax25_deframer.h>

typedef struct ax25tnc_demodulator
{
    ms_fsk_detector_t fsk_detector;
    ms_bit_detector_t bit_detector;
    ms_linecode_nrzi_decoder_t nrzi_decoder;
    hs_ax25_deframer_t ax25_deframer;
    void (*packet_callback)(hs_ax25_packet_t *packet);

} ax25tnc_demodulator_t;

/**
 * Initialize the demodulator.
 *
 * @param demod The demodulator to initialize.
 * @param config The configuration to use.
 * @return 0 on success, 1 on error.
 */
int ax25tnc_demodulator_init(
    ax25tnc_demodulator_t *demod,
    ax25tnc_config_t *config);

/**
 * Set the callbacks for the demodulator.
 *
 * @param demod The demodulator.
 * @param packet_callback The callback for received packets.
 */
void ax25tnc_demodulator_set_callbacks(
    ax25tnc_demodulator_t *demod,
    void (*packet_callback)(hs_ax25_packet_t *packet));

/**
 * Process samples.
 *
 * @param demod The demodulator.
 * @param samples The samples to process.
 * @param samples_count The number of samples.
 */
void ax25tnc_demodulator_process(
    ax25tnc_demodulator_t *demod,
    ms_float *samples,
    int samples_count);

/**
 * Destroy the demodulator.
 *
 * @param demod The demodulator to destroy.
 */
void ax25tnc_demodulator_destroy(
    ax25tnc_demodulator_t *demod);

#endif // _AX25TNC_DEMODULATOR_H