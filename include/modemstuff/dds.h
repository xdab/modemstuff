#ifndef _MODEMSTUFF_DDS_H
#define _MODEMSTUFF_DDS_H

#include <modemstuff/modemstuff.h> 

typedef struct ms_dds {
    ms_float sample_rate;
    ms_float frequency;
    ms_float phase;
} ms_dds_t;

/**
 * Initialize the DDS.
 *
 * @param dds The DDS to initialize.
 * @param sample_rate Sample rate.
 * @param frequency Initial frequency.
 * @param phase Initial phase.
 */
void ms_dds_init(ms_dds_t *dds, ms_float sample_rate, ms_float frequency, ms_float phase);
ms_float ms_dds_get_sample(ms_dds_t *dds);

#endif // _MODEMSTUFF_DDS_H