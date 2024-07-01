#include <modemstuff/fsk.h>

#include <stdlib.h>

#define PRE_BPF_ORDER 8
#define PRE_BPF_WIDTH_MULTIPLIER 0.750

#define SYM_CLIPPING 0.5

#define POST_LPF_ORDER 4
#define POST_LPF_BAUD_RATE_MULTIPLIER 0.900

#if __DETECTOR == __IQ_DETECTOR
#define IQ_LPF_ORDER 2
#define IQ_LPF_BAUD_RATE_MULTIPLIER 0.875

#elif __DETECTOR == __GOERTZEL_DETECTOR
#define GOERTZEL_WINDOW_SIZE(S, B) ((int)((S) / (B)))
#define GOERTZEL_WINDOW_BUFFER_OVERSIZING 10
#define SYMBOL_AGC_ATTACK 0.33
#define SYMBOL_AGC_DECAY 0.0007
#endif

#include <stdio.h>  

int ms_fsk_detector_init(ms_fsk_detector_t *det, ms_float mark_freq, ms_float space_freq, ms_float baud_rate, ms_float sample_rate)
{
    ms_float pre_filter_width;

    det->center_freq = (mark_freq + space_freq) / 2.0;
    det->sample_rate = sample_rate;

    // Pre-filter
    pre_filter_width = ms_abs(mark_freq - space_freq) * PRE_BPF_WIDTH_MULTIPLIER;
    ms_butterworth_bpf_init(&det->pre_filter, PRE_BPF_ORDER, det->center_freq - pre_filter_width, det->center_freq + pre_filter_width, sample_rate);

// Main detector
#if __DETECTOR == __IQ_DETECTOR
    det->last_phase = 0.0;
    ms_dds_init(&det->i_dds, sample_rate, det->center_freq, 0.0);
    ms_dds_init(&det->q_dds, sample_rate, det->center_freq, M_PI / 2.0);
    ms_butterworth_lpf_init(&det->i_lpf, IQ_LPF_ORDER, baud_rate * IQ_LPF_BAUD_RATE_MULTIPLIER, sample_rate);
    ms_butterworth_lpf_init(&det->q_lpf, IQ_LPF_ORDER, baud_rate * IQ_LPF_BAUD_RATE_MULTIPLIER, sample_rate);

#elif __DETECTOR == __GOERTZEL_DETECTOR
    det->mark_freq = mark_freq;
    det->mark_peak = 0.0;
    det->space_freq = space_freq;
    det->space_peak = 0.0;
    ms_window_init(&det->sample_window,
                   GOERTZEL_WINDOW_SIZE(sample_rate, baud_rate),
                   GOERTZEL_WINDOW_BUFFER_OVERSIZING);
    printf("GOERTZEL_WINDOW_SIZE: %d\n", GOERTZEL_WINDOW_SIZE(sample_rate, baud_rate));
    
#endif

    // Post-filter
    ms_butterworth_lpf_init(&det->post_filter, POST_LPF_ORDER, baud_rate * POST_LPF_BAUD_RATE_MULTIPLIER, sample_rate);

    // Process one non-zero sample to properly initialize the filters
    ms_fsk_detector_process(det, 1e-3);

    return 0;
}

ms_float ms_fsk_detector_process(ms_fsk_detector_t *det, ms_float sample)
{
    ms_float i, q, phase, phase_diff, symbol;

    // Pre-filter
    sample = ms_butterworth_bpf_process(&det->pre_filter, sample);

    // Main detector
#if __DETECTOR == __IQ_DETECTOR
    // IQ decomposition
    i = ms_dds_get_sample(&det->i_dds) * sample;
    i = ms_butterworth_lpf_process(&det->i_lpf, i);
    q = ms_dds_get_sample(&det->q_dds) * sample;
    q = ms_butterworth_lpf_process(&det->q_lpf, q);

    // Phase demodulation
    phase = ms_atan2(q, i);

    // Frequency demodulation
    phase_diff = phase - det->last_phase;
    det->last_phase = phase;

    // Wrap phase difference to -PI to PI
    while (phase_diff > M_PI)
        phase_diff -= 2.0 * M_PI;
    while (phase_diff < -M_PI)
        phase_diff += 2.0 * M_PI;

    // Normalize phase difference to symbol
    symbol = phase_diff;
    symbol /= M_PI * det->center_freq / det->sample_rate;

#elif __DETECTOR == __GOERTZEL_DETECTOR
    // Push sample to window
    ms_window_push(&det->sample_window, sample);

    // Goertzel algorithm to determine mark and space symbol strengths
    ms_float mark_power = ms_goertzel(
        det->sample_window.buf + det->sample_window.head,
        det->sample_window.size,
        det->sample_rate,
        det->mark_freq,
        NULL);

    ms_float space_power = ms_goertzel(
        det->sample_window.buf + det->sample_window.head,
        det->sample_window.size,
        det->sample_rate,
        det->space_freq,
        NULL);

    // Automatic gain control for symbol strengths
    if (mark_power >= det->mark_peak)
        det->mark_peak = mark_power * SYMBOL_AGC_ATTACK + det->mark_peak * (1.0 - SYMBOL_AGC_ATTACK);
    else
        det->mark_peak = mark_power * SYMBOL_AGC_DECAY + det->mark_peak * (1.0 - SYMBOL_AGC_DECAY);
    mark_power /= det->mark_peak;

    if (space_power >= det->space_peak)
        det->space_peak = space_power * SYMBOL_AGC_ATTACK + det->space_peak * (1.0 - SYMBOL_AGC_ATTACK);
    else
        det->space_peak = space_power * SYMBOL_AGC_DECAY + det->space_peak * (1.0 - SYMBOL_AGC_DECAY);
    space_power /= det->space_peak;

    // Symbol strength comparison
    symbol = mark_power - space_power;
#else

    // Whatever, this should not happen
    symbol = sample;
#endif

    // Symbol clipping
    if (symbol > SYM_CLIPPING)
        symbol = SYM_CLIPPING;
    else if (symbol < -SYM_CLIPPING)
        symbol = -SYM_CLIPPING;

    // Post-filter
    symbol = ms_butterworth_lpf_process(&det->post_filter, symbol);

    return symbol;
}

void ms_fsk_detector_destroy(ms_fsk_detector_t *det)
{
    ms_butterworth_bpf_destroy(&det->pre_filter);
    ms_butterworth_lpf_destroy(&det->post_filter);

#if __DETECTOR == __IQ_DETECTOR
    ms_butterworth_lpf_destroy(&det->i_lpf);
    ms_butterworth_lpf_destroy(&det->q_lpf);
#endif
}

int ms_fsk_generator_init(ms_fsk_generator_t *gen, ms_float mark_freq, ms_float space_freq, ms_float baud_rate, ms_float sample_rate)
{
    gen->baud_rate = baud_rate;
    gen->sample_rate = sample_rate;
    gen->mark_freq = mark_freq;
    gen->space_freq = space_freq;

    ms_dds_init(&gen->fsk_dds, sample_rate, mark_freq, 0.0);

    return 0;
}

int ms_fsk_generator_process(ms_fsk_generator_t *gen, ms_bit bit, ms_float *out_samples, int out_samples_size)
{
    int i, samples_count;

    samples_count = gen->sample_rate / gen->baud_rate; // TODO even out rounding errors

    gen->fsk_dds.frequency = (bit == MS_BIT_ONE) ? gen->mark_freq : gen->space_freq;

    for (i = 0; i < samples_count; i++)
        out_samples[i] = ms_dds_get_sample(&gen->fsk_dds);

    return samples_count;
}

void ms_fsk_generator_destroy(ms_fsk_generator_t *gen)
{
}
