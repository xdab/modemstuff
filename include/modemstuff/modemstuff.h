#ifndef _MODEMSTUFF_MODEMSTUFF_H
#define _MODEMSTUFF_MODEMSTUFF_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

typedef float ms_float;

#define ms_abs fabsf
#define ms_max fmaxf
#define ms_min fminf
#define ms_floor floorf
#define ms_ceil ceilf
#define ms_sin sinf
#define ms_cos cosf
#define ms_tan tanf
#define ms_atan2 atan2f
#define ms_sqrt sqrtf
#define ms_round roundf
#define ms_exp expf

typedef signed char ms_bit;

#define MS_BIT_NONE ((ms_bit)-1)
#define MS_BIT_ZERO ((ms_bit)0)
#define MS_BIT_ONE ((ms_bit)1)

typedef unsigned char ms_byte;

#endif // _MODEMSTUFF_MODEMSTUFF_H