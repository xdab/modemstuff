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

int ms_fsk_detector_init(ms_fsk_detector_t *det, ms_float mark_freq, ms_float space_freq, ms_float baud_rate, ms_float sample_rate);

ms_float ms_fsk_detector_process(ms_fsk_detector_t *det, ms_float sample);

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

int ms_fsk_generator_init(ms_fsk_generator_t *gen, ms_float mark_freq, ms_float space_freq, ms_float baud_rate, ms_float sample_rate);

int ms_fsk_generator_process(ms_fsk_generator_t *gen, ms_bit bit, ms_float *out_samples, int out_samples_size);

void ms_fsk_generator_destroy(ms_fsk_generator_t *gen);

#endif // _MODEMSTUFF_FSK_H