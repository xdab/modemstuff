#include <modemstuff/butterworth.h>

#include <stdio.h>
#include <stdlib.h>

static inline ms_float *_ms_malloc(int n)
{
    int i;
    ms_float *array;

    array = (ms_float *)malloc(n * sizeof(ms_float));
    if (array == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    return array;
}

static inline ms_float *_ms_calloc(int n)
{
    int i;
    ms_float *array;

    array = _ms_malloc(n);
    for (i = 0; i < n; i++)
        array[i] = 0.0;

    return array;
}

void ms_butterworth_lpf_init(ms_butterworth_lpf_t *lpf, int order, ms_float cutoff_freq, ms_float sample_rate)
{
    ms_float a, a2, f, r, s;
    int i;

    lpf->n = order / 2;
    lpf->A = _ms_malloc(lpf->n);
    lpf->d1 = _ms_malloc(lpf->n);
    lpf->d2 = _ms_malloc(lpf->n);
    lpf->w0 = _ms_calloc(lpf->n);
    lpf->w1 = _ms_calloc(lpf->n);
    lpf->w2 = _ms_calloc(lpf->n);

    s = sample_rate;
    f = cutoff_freq;
    a = ms_tan(M_PI * f / s);
    a2 = a * a;

    for (i = 0; i < lpf->n; i++)
    {
        r = ms_sin(M_PI * (2.0 * i + 1.0) / (4.0 * lpf->n));
        s = a2 + 2.0 * r * a + 1.0;
        lpf->A[i] = a2 / s;
        lpf->d1[i] = 2.0 * (1.0 - a2) / s;
        lpf->d2[i] = -(a2 - 2.0 * r * a + 1.0) / s;
    }
}

ms_float ms_butterworth_lpf_process(ms_butterworth_lpf_t *lpf, ms_float sample)
{
    int i;

    for (i = 0; i < lpf->n; i++)
    {
        lpf->w0[i] = lpf->d1[i] * lpf->w1[i] + lpf->d2[i] * lpf->w2[i] + sample;
        sample = lpf->A[i] * (lpf->w0[i] + 2.0 * lpf->w1[i] + lpf->w2[i]);
        lpf->w2[i] = lpf->w1[i];
        lpf->w1[i] = lpf->w0[i];
    }

    return sample;
}

void ms_butterworth_lpf_destroy(ms_butterworth_lpf_t *lpf)
{
    free(lpf->A);
    free(lpf->d1);
    free(lpf->d2);
    free(lpf->w0);
    free(lpf->w1);
    free(lpf->w2);
}

void ms_butterworth_hpf_init(ms_butterworth_hpf_t *hpf, int order, ms_float cutoff_freq, ms_float sample_rate)
{
    ms_float a, a2, f, r, s;
    int i;

    hpf->n = order / 2;
    hpf->A = _ms_malloc(hpf->n);
    hpf->d1 = _ms_malloc(hpf->n);
    hpf->d2 = _ms_malloc(hpf->n);
    hpf->w0 = _ms_calloc(hpf->n);
    hpf->w1 = _ms_calloc(hpf->n);
    hpf->w2 = _ms_calloc(hpf->n);

    s = sample_rate;
    f = cutoff_freq;
    a = ms_tan(M_PI * f / s);
    a2 = a * a;

    for (i = 0; i < hpf->n; i++)
    {
        r = ms_sin(M_PI * (2.0 * i + 1.0) / (4.0 * hpf->n));
        s = a2 + 2.0 * r * a + 1.0;
        hpf->A[i] = 1.0 / s;
        hpf->d1[i] = 2.0 * (1.0 - a2) / s;
        hpf->d2[i] = -(a2 - 2.0 * r * a + 1.0) / s;
    }
}

ms_float ms_butterworth_hpf_process(ms_butterworth_hpf_t *hpf, ms_float sample)
{
    int i;

    for (i = 0; i < hpf->n; i++)
    {
        hpf->w0[i] = hpf->d1[i] * hpf->w1[i] + hpf->d2[i] * hpf->w2[i] + sample;
        sample = hpf->A[i] * (hpf->w0[i] - 2.0 * hpf->w1[i] + hpf->w2[i]);
        hpf->w2[i] = hpf->w1[i];
        hpf->w1[i] = hpf->w0[i];
    }

    return sample;
}

void ms_butterworth_bpf_init(ms_butterworth_bpf_t *bpf, int order, ms_float low_cutoff_freq, ms_float high_cutoff_freq, ms_float sample_rate)
{
    ms_float a, a2, b, b2, fl, fu, r, s;
    int i;

    bpf->n = order / 4;
    bpf->A = _ms_malloc(bpf->n);
    bpf->d1 = _ms_malloc(bpf->n);
    bpf->d2 = _ms_malloc(bpf->n);
    bpf->d3 = _ms_malloc(bpf->n);
    bpf->d4 = _ms_malloc(bpf->n);
    bpf->w0 = _ms_calloc(bpf->n);
    bpf->w1 = _ms_calloc(bpf->n);
    bpf->w2 = _ms_calloc(bpf->n);
    bpf->w3 = _ms_calloc(bpf->n);
    bpf->w4 = _ms_calloc(bpf->n);

    s = sample_rate;
    fl = low_cutoff_freq;
    fu = high_cutoff_freq;
    a = ms_cos(M_PI * (fu + fl) / s) / ms_cos(M_PI * (fu - fl) / s);
    b = ms_tan(M_PI * (fu - fl) / s);
    a2 = a * a;
    b2 = b * b;

    for (i = 0; i < bpf->n; i++)
    {
        r = ms_sin(M_PI * (2.0 * i + 1.0) / (4.0 * bpf->n));
        s = b2 + 2.0 * r * b + 1.0;
        bpf->A[i] = b2 / s;
        bpf->d1[i] = 4.0 * a * (1.0 + b * r) / s;
        bpf->d2[i] = 2.0 * (b2 - 2.0 * a2 - 1.0) / s;
        bpf->d3[i] = 4.0 * a * (1.0 - b * r) / s;
        bpf->d4[i] = -(b2 - 2.0 * r * b + 1.0) / s;
    }
}

ms_float ms_butterworth_bpf_process(ms_butterworth_bpf_t *bpf, ms_float sample)
{
    int i;

    for (i = 0; i < bpf->n; i++)
    {
        bpf->w0[i] = bpf->d1[i] * bpf->w1[i] + bpf->d2[i] * bpf->w2[i] + bpf->d3[i] * bpf->w3[i] + bpf->d4[i] * bpf->w4[i] + sample;
        sample = bpf->A[i] * (bpf->w0[i] - 2.0 * bpf->w2[i] + bpf->w4[i]);
        bpf->w4[i] = bpf->w3[i];
        bpf->w3[i] = bpf->w2[i];
        bpf->w2[i] = bpf->w1[i];
        bpf->w1[i] = bpf->w0[i];
    }

    return sample;
}

void ms_butterworth_bpf_destroy(ms_butterworth_bpf_t *bpf)
{
    free(bpf->A);
    free(bpf->d1);
    free(bpf->d2);
    free(bpf->d3);
    free(bpf->d4);
    free(bpf->w0);
    free(bpf->w1);
    free(bpf->w2);
    free(bpf->w3);
    free(bpf->w4);
}

void ms_butterworth_bsf_init(ms_butterworth_bsf_t *bsf, int order, ms_float low_cutoff_freq, ms_float high_cutoff_freq, ms_float sample_rate)
{
    ms_float a, a2, b, b2, fl, fu, r, s;
    int i;

    bsf->n = order / 4;
    bsf->A = _ms_malloc(bsf->n);
    bsf->d1 = _ms_malloc(bsf->n);
    bsf->d2 = _ms_malloc(bsf->n);
    bsf->d3 = _ms_malloc(bsf->n);
    bsf->d4 = _ms_malloc(bsf->n);
    bsf->w0 = _ms_calloc(bsf->n);
    bsf->w1 = _ms_calloc(bsf->n);
    bsf->w2 = _ms_calloc(bsf->n);
    bsf->w3 = _ms_calloc(bsf->n);
    bsf->w4 = _ms_calloc(bsf->n);

    s = sample_rate;
    fl = low_cutoff_freq;
    fu = high_cutoff_freq;
    a = ms_cos(M_PI * (fu + fl) / s) / ms_cos(M_PI * (fu - fl) / s);
    b = ms_tan(M_PI * (fu - fl) / s);
    a2 = a * a;
    b2 = b * b;

    for (i = 0; i < bsf->n; i++)
    {
        r = ms_sin(M_PI * (2.0 * i + 1.0) / (4.0 * bsf->n));
        s = b2 + 2.0 * b * r + 1.0;
        bsf->A[i] = 1.0 / s;
        bsf->d1[i] = 4.0 * a * (1.0 + b * r) / s;
        bsf->d2[i] = 2.0 * (b2 - 2.0 * a2 - 1.0) / s;
        bsf->d3[i] = 4.0 * a * (1.0 - b * r) / s;
        bsf->d4[i] = -(b2 - 2.0 * b * r + 1.0) / s;
    }

    bsf->r = 4.0 * a;
    bsf->s = 4.0 * a2 + 2.0;
}

ms_float ms_butterworth_bsf_process(ms_butterworth_bsf_t *bsf, ms_float sample)
{
    int i;

    for (i = 0; i < bsf->n; i++)
    {
        bsf->w0[i] = bsf->d1[i] * bsf->w1[i] + bsf->d2[i] * bsf->w2[i] + bsf->d3[i] * bsf->w3[i] + bsf->d4[i] * bsf->w4[i] + sample;
        sample = bsf->A[i] * (bsf->w0[i] - bsf->r * bsf->w1[i] + bsf->s * bsf->w2[i] - bsf->r * bsf->w3[i] + bsf->w4[i]);
        bsf->w4[i] = bsf->w3[i];
        bsf->w3[i] = bsf->w2[i];
        bsf->w2[i] = bsf->w1[i];
        bsf->w1[i] = bsf->w0[i];
    }

    return sample;
}