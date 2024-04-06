#ifndef MODEMSTUFF_APPS_APP_AUDIO_H
#define MODEMSTUFF_APPS_APP_AUDIO_H

#define MAX_AUDIO_DEVICES (32)
#define AUDIO_DEVICE_MAX_NAME_LEN (256)

#include <portaudio.h>

typedef struct audio_device
{
    int index;
    char name[AUDIO_DEVICE_MAX_NAME_LEN + 1];
    int sample_rate;
    int port;
    PaStream *stream;
} audio_device_t;

int audio_init(audio_device_t audio_devices[MAX_AUDIO_DEVICES]);

int audio_open(audio_device_t *audio_device, PaStreamCallback *stream_callback, void *user_data);

int audio_close(audio_device_t *audio_device);

int audio_destroy(audio_device_t audio_devices[MAX_AUDIO_DEVICES], int num_audio_devices);

#endif //MODEMSTUFF_APPS_APP_AUDIO_H