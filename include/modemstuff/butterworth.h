#ifndef _MODEMSTUFF_BUTTERWORTH_H
#define _MODEMSTUFF_BUTTERWORTH_H

#include <modemstuff/modemstuff.h>

struct ms_butterworth1_s
{
    int n;
    ms_float *A;
    ms_float *d1;
    ms_float *d2;
    ms_float *w0;
    ms_float *w1;
    ms_float *w2;
};

typedef struct ms_butterworth1_s ms_butterworth_lpf_or_hpf_t;
typedef struct ms_butterworth1_s ms_butterworth_lpf_t;
typedef struct ms_butterworth1_s ms_butterworth_hpf_t;

void ms_butterworth_lpf_init(ms_butterworth_lpf_t *lpf, int order, ms_float cutoff_freq, ms_float sample_rate);
ms_float ms_butterworth_lpf_process(ms_butterworth_lpf_t *lpf, ms_float sample);
void ms_butterworth_lpf_destroy(ms_butterworth_lpf_t *lpf);

void ms_butterworth_hpf_init(ms_butterworth_hpf_t *hpf, int order, ms_float cutoff_freq, ms_float sample_rate);
ms_float ms_butterworth_hpf_process(ms_butterworth_hpf_t *hpf, ms_float sample);
static inline void ms_butterworth_hpf_destroy(ms_butterworth_hpf_t *hpf)
{
    ms_butterworth_lpf_destroy((ms_butterworth_lpf_t *)hpf);
}

typedef struct ms_butterworth_bpf
{
    int n;
    ms_float *A;
    ms_float *d1;
    ms_float *d2;
    ms_float *d3;
    ms_float *d4;
    ms_float *w0;
    ms_float *w1;
    ms_float *w2;
    ms_float *w3;
    ms_float *w4;
} ms_butterworth_bpf_t;

void ms_butterworth_bpf_init(ms_butterworth_bpf_t *bpf, int order, ms_float low_cutoff_freq, ms_float high_cutoff_freq, ms_float sample_rate);
float ms_butterworth_bpf_process(ms_butterworth_bpf_t *bpf, ms_float sample);
void ms_butterworth_bpf_destroy(ms_butterworth_bpf_t *bpf);

typedef struct ms_butterworth_bsf
{
    int n;
    ms_float *A;
    ms_float *d1;
    ms_float *d2;
    ms_float *d3;
    ms_float *d4;
    ms_float *w0;
    ms_float *w1;
    ms_float *w2;
    ms_float *w3;
    ms_float *w4;
    ms_float r;
    ms_float s;
} ms_butterworth_bsf_t;

void ms_butterworth_bsf_init(ms_butterworth_bsf_t *bsf, int order, ms_float low_cutoff_freq, ms_float high_cutoff_freq, ms_float sample_rate);
float ms_butterworth_bsf_process(ms_butterworth_bsf_t *bsf, ms_float sample);
#define ms_butterworth_bsf_destroy ms_butterworth_bpf_destroy

#endif // _MODEMSTUFF_BUTTERWORTH_H