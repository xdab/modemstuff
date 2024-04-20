#ifndef _MODEMSTUFF_FSK_H
#define _MODEMSTUFF_FSK_H

#include <modemstuff/modemstuff.h>
#include <modemstuff/butterworth.h>
#include <modemstuff/dds.h>

// Detector algorithm "enum"
#define __IQ_DETECTOR 1
#define __GOERTZEL_DETECTOR 2

// Select detector algorithm
#define __DETECTOR __GOERTZEL_DETECTOR

#if __DETECTOR == __IQ_DETECTOR

#elif __DETECTOR == __GOERTZEL_DETECTOR
#include <modemstuff/window.h>
#include <modemstuff/goertzel.h>
#endif

// FSK detector
// Input: raw audio samples
// Output: relative mark/space tone strength

typedef struct ms_fsk_detector
{
    ms_float sample_rate;
    ms_float center_freq;
    ms_butterworth_bpf_t pre_filter;
    ms_butterworth_lpf_t post_filter;

#if __DETECTOR == __IQ_DETECTOR
    ms_dds_t i_dds, q_dds;
    ms_butterworth_lpf_t i_lpf, q_lpf;
    ms_float last_phase;
#elif __DETECTOR == __GOERTZEL_DETECTOR
    ms_float mark_freq;
    ms_float space_freq;
    ms_float mark_peak;
    ms_float space_peak;
    ms_window_t sample_window;
#endif

} ms_fsk_detector_t;

/**
 * Initialize the FSK detector.
 *
 * @param det The FSK detector to initialize.
 * @param mark_freq Mark frequency.
 * @param space_freq Space frequency.
 * @param baud_rate Baud rate.
 * @param sample_rate Sample rate.
 *
 * @return 0 if successful, 1 otherwise.
 */
int ms_fsk_detector_init(ms_fsk_detector_t *det, ms_float mark_freq, ms_float space_freq, ms_float baud_rate, ms_float sample_rate);

/**
 * Process a sample.
 *
 * @param det The FSK detector.
 * @param sample The sample to process.
 *
 * @return The relative mark/space tone strength = soft symbol.
 */
ms_float ms_fsk_detector_process(ms_fsk_detector_t *det, ms_float sample);

/**
 * Destroy the FSK detector.
 *
 * @param det The FSK detector to destroy.
 */
void ms_fsk_detector_destroy(ms_fsk_detector_t *det);

// FSK generator
// Input: symbol
// Output: samples representing the symbol

typedef struct ms_fsk_generator
{
    ms_float mark_freq;
    ms_float space_freq;
    ms_float baud_rate;
    ms_float sample_rate;
    ms_dds_t fsk_dds;

} ms_fsk_generator_t;

/**
 * Initialize the FSK generator.
 *
 * @param gen The FSK generator to initialize.
 * @param mark_freq Mark frequency.
 * @param space_freq Space frequency.
 * @param baud_rate Baud rate.
 * @param sample_rate Sample rate.
 *
 * @return 0 if successful, 1 otherwise.
 */
int ms_fsk_generator_init(ms_fsk_generator_t *gen, ms_float mark_freq, ms_float space_freq, ms_float baud_rate, ms_float sample_rate);

/**
 * Modulate a bit.
 *
 * @param gen The FSK generator.
 * @param bit The bit to modulate.
 * @param out_samples The output samples.
 * @param out_samples_size The size of the output samples buffer.
 *
 * @return The number of samples written to the output buffer.
 */
int ms_fsk_generator_process(ms_fsk_generator_t *gen, ms_bit bit, ms_float *out_samples, int out_samples_size);

/**
 * Destroy the FSK generator.
 *
 * @param gen The FSK generator to destroy.
 */
void ms_fsk_generator_destroy(ms_fsk_generator_t *gen);

#endif // _MODEMSTUFF_FSK_H