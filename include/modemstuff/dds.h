#ifndef _MODEMSTUFF_DDS_H
#define _MODEMSTUFF_DDS_H

#include <modemstuff/modemstuff.h> 

typedef struct ms_dds {
    ms_float sample_rate;
    ms_float frequency;
    ms_float phase;
} ms_dds_t;

void ms_dds_init(ms_dds_t *dds, ms_float sample_rate, ms_float frequency, ms_float phase);
ms_float ms_dds_get_sample(ms_dds_t *dds);

#endif // _MODEMSTUFF_DDS_H