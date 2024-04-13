#include <modemstuff/dds.h>

void ms_dds_init(ms_dds_t *dds, ms_float sample_rate, ms_float frequency, ms_float phase)
{
    dds->sample_rate = sample_rate;
    dds->frequency = frequency;
    dds->phase = phase;
}

ms_float ms_dds_get_sample(ms_dds_t *dds)
{
    ms_float omega = 2.0 * M_PI * dds->frequency / dds->sample_rate;
    ms_float sample = ms_sin(omega + dds->phase);
    dds->phase += omega;
    if (dds->phase > 2.0 * M_PI)
        dds->phase -= 2.0 * M_PI;
    return sample;
}
