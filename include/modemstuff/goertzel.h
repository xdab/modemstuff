#ifndef _MODEMSTUFF_GOERTZEL_H
#define _MODEMSTUFF_GOERTZEL_H

#include <modemstuff/modemstuff.h>

ms_float ms_goertzel(ms_float *samples, int num_samples, ms_float sample_rate, ms_float frequency, ms_float *window);

#endif // _MODEMSTUFF_GOERTZEL_H