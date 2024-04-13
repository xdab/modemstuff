#include <modemstuff/window.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ms_window_init(ms_window_t *window, int size, int buf_oversizing)
{
    int i;

    window->buf_size = size * buf_oversizing + 1;
    window->buf = (ms_float *)malloc(window->buf_size * sizeof(ms_float));
    if (window->buf == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < size; i++)
        window->buf[i] = 0.0;

    window->size = size;
    window->head = 0;
    window->tail = size;
}

void ms_window_push(ms_window_t *window, ms_float sample)
{
    if (window->head == window->buf_size - window->size)
    {
        memcpy(window->buf, window->buf + window->head + 1, window->size * sizeof(ms_float));
        window->head = -1;
        window->tail = window->size - 1;
    }

    window->head++;
    window->buf[window->tail++] = sample;
}

void ms_window_destroy(ms_window_t *window)
{
    free(window->buf);
}

void ms_window_blackman_harris(ms_float *window, int size)
{
    const ms_float a0 = 0.35875;
    const ms_float a1 = 0.48829;
    const ms_float a2 = 0.14128;
    const ms_float a3 = 0.01168;

    int n;

    for (n = 0; n < size; n++)
    {
        window[n] = a0 - a1 * ms_cos((2.0 * M_PI * n) / (size - 1)) + a2 * ms_cos((4.0 * M_PI * n) / (size - 1)) - a3 * ms_cos((6.0 * M_PI * n) / (size - 1));
    }
}

void ms_window_hann(ms_float *window, int size)
{
    int n;

    for (n = 0; n < size; n++)
    {
        window[n] = 0.5 * (1.0 - ms_cos((2.0 * M_PI * n) / (size - 1)));
    }
}