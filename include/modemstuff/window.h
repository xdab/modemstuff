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

/**
 * Initialize a window.
 *
 * @param window The window to initialize.
 * @param size The size of the window.
 * @param buf_oversizing The oversizing factor of the buffer.
 */
void ms_window_init(ms_window_t *window, int size, int buf_oversizing);

/**
 * Push a value into the window.
 *
 * @param window The window.
 * @param value The value to push.
 */
void ms_window_push(ms_window_t *window, ms_float value);

/**
 * Destroy the window.
 * 
 * @param window The window to destroy.
 */
void ms_window_destroy(ms_window_t *window);

// TODO move

/**
 * Apply a Blackman-Harris window to a buffer.
 *
 * @param window The buffer to apply the window to.
 * @param size The size of the buffer.
 */
void ms_window_blackman_harris(ms_float *window, int size);

/**
 * Apply a Hann window to a buffer.
 *
 * @param window The buffer to apply the window to.
 * @param size The size of the buffer.
 */
void ms_window_hann(ms_float *window, int size);

#endif // _MODEMSTUFF_WINDOW_H