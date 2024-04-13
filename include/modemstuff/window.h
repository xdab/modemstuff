#ifndef _MODEMSTUFF_WINDOW_H
#define _MODEMSTUFF_WINDOW_H

#include <modemstuff/modemstuff.h>

typedef struct ms_window {
    ms_float *buf;
    int buf_size;
    int size;
    int head;
    int tail;
} ms_window_t;

void ms_window_init(ms_window_t *window, int size, int buf_oversizing);

void ms_window_push(ms_window_t *window, ms_float value);

void ms_window_destroy(ms_window_t *window);

// TODO move
void ms_window_blackman_harris(ms_float *window, int size);

void ms_window_hann(ms_float *window, int size);

#endif // _MODEMSTUFF_WINDOW_H