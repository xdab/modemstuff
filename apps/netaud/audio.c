#include "audio.h"

#include <stdio.h>
#include <string.h>
#include <portaudio.h>

#define FRAMES_PER_BUFFER paFramesPerBufferUnspecified

int audio_init(audio_device_t audio_devices[MAX_AUDIO_DEVICES])
{
    PaError err;
    const PaDeviceInfo *pa_device_info;
    int num_devices;
    int i;

    err = Pa_Initialize();
    if (err != paNoError)
    {
        fprintf(stderr, "Error: Pa_Initialize() failed: %s\n", Pa_GetErrorText(err));
        return -1;
    }

    num_devices = Pa_GetDeviceCount();
    if (num_devices < 0)
    {
        fprintf(stderr, "Error: Pa_GetDeviceCount() failed: %s\n", Pa_GetErrorText(num_devices));
        return -1;
    }

    if (num_devices > MAX_AUDIO_DEVICES)
    {
        fprintf(stderr, "Error: Too many audio devices: %d\n", num_devices);
        return -1;
    }

    for (i = 0; i < num_devices; i++)
    {
        pa_device_info = Pa_GetDeviceInfo(i);
        if (pa_device_info == NULL)
        {
            fprintf(stderr, "Error: Pa_GetDeviceInfo() failed: %s\n", Pa_GetErrorText(i));
            return -1;
        }

        audio_devices[i].index = i;
        strncpy(audio_devices[i].name, pa_device_info->name, AUDIO_DEVICE_MAX_NAME_LEN);
        audio_devices[i].name[AUDIO_DEVICE_MAX_NAME_LEN] = '\0';
    }

    return num_devices;
}

int audio_open(audio_device_t *audio_device, PaStreamCallback *stream_callback, void *user_data)
{
    PaError err;
    PaStreamParameters input_parameters, output_parameters;

    input_parameters.device = audio_device->index;
    input_parameters.channelCount = 1;
    input_parameters.sampleFormat = paFloat32;
    input_parameters.suggestedLatency = Pa_GetDeviceInfo(input_parameters.device)->defaultLowInputLatency;
    input_parameters.hostApiSpecificStreamInfo = NULL;

    output_parameters.device = audio_device->index;
    output_parameters.channelCount = 1;
    output_parameters.sampleFormat = paFloat32;
    output_parameters.suggestedLatency = Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
    output_parameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&audio_device->stream, &input_parameters, &output_parameters, audio_device->sample_rate, FRAMES_PER_BUFFER, paClipOff, stream_callback, user_data);
    if (err != paNoError)
    {
        fprintf(stderr, "Error: Pa_OpenStream() failed: %s\n", Pa_GetErrorText(err));
        return -1;
    }

    err = Pa_StartStream(audio_device->stream);
    if (err != paNoError)
    {
        fprintf(stderr, "Error: Pa_StartStream() failed: %s\n", Pa_GetErrorText(err));
        return -1;
    }

    return 0;
}

int audio_close(audio_device_t *audio_device)
{
    PaError err;

    err = Pa_StopStream(audio_device->stream);
    if (err != paNoError)
    {
        fprintf(stderr, "Error: Pa_StopStream() failed: %s\n", Pa_GetErrorText(err));
        return -1;
    }

    return 0;
}

int audio_destroy(audio_device_t audio_devices[MAX_AUDIO_DEVICES], int num_audio_devices)
{
    PaError err;
    int i;

    for (i = 0; i < num_audio_devices; i++)
    {
        if (audio_devices[i].stream == NULL)
        {
            continue;
        }

        err = Pa_CloseStream(audio_devices[i].stream);
        if (err != paNoError)
        {
            fprintf(stderr, "Error: Pa_CloseStream() failed: %s\n", Pa_GetErrorText(err));
            return -1;
        }

        memset(&audio_devices[i], 0, sizeof(audio_device_t));
    }

    err = Pa_Terminate();
    if (err != paNoError)
    {
        fprintf(stderr, "Error: Pa_Terminate() failed: %s\n", Pa_GetErrorText(err));
        return -1;
    }

    return 0;
}