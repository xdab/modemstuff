#ifndef _MODEMSTUFF_GOERTZEL_H
#define _MODEMSTUFF_GOERTZEL_H

#include <modemstuff/modemstuff.h>

/**
 * Goertzel algorithm.
 *
 * @param samples Samples.
 * @param num_samples Number of samples.
 * @param sample_rate Sample rate.
 * @param frequency Frequency to detect.
 * @param window Optional window to apply to the samples.
 *
 * @return Magnitude of the frequency in the samples.
 */
ms_float ms_goertzel(ms_float *samples, int num_samples, ms_float sample_rate, ms_float frequency, ms_float *window);

#endif // _MODEMSTUFF_GOERTZEL_H