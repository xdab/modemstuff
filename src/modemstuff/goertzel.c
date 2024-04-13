#include <modemstuff/goertzel.h>

ms_float ms_goertzel(ms_float *samples, int num_samples, ms_float sample_rate, ms_float frequency, ms_float *window)
{
    int i, k;
    ms_float coeff, power;
    ms_float q0, q1, q2;

    k = (int)(0.5 + ((((ms_float)num_samples) * frequency) / sample_rate));
    coeff = 2.0 * ms_cos((2.0 * M_PI * k) / ((ms_float)num_samples));

    q0 = q1 = q2 = 0;

    if (window)
    {
        for (i = 0; i < num_samples; i++)
        {
            q0 = coeff * q1 - q2 + samples[i] * window[i];
            q2 = q1;
            q1 = q0;
        }
    }
    else
    {
        for (i = 0; i < num_samples; i++)
        {
            q0 = coeff * q1 - q2 + samples[i];
            q2 = q1;
            q1 = q0;
        }
    }

    power = q1 * q1 + q2 * q2 - q1 * q2 * coeff;
    power /= num_samples;
    power /= 2.0;
    return power;
}