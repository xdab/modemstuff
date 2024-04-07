#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <modemstuff/netstuff/netserver.h>
#include <modemstuff/datastuff/linkedbuffer.h>

#include "audio.h"
#include "utils.h"

#define ARGS_USED (3)
#define ARGS_EXPECTED (1 + ARGS_USED)
#define ARG_DEVICE_NAME 1
#define ARG_SAMPLE_RATE 2
#define ARG_TCP_PORT 3

double start_time = -1.0;
pthread_mutex_t client_data_mutex = PTHREAD_MUTEX_INITIALIZER;
ds_linked_buffer_t client_data_buffer;

int main(char argc, const char *argv[]);
audio_device_t *parse_args(int argc, const char *argv[], audio_device_t audio_devices[MAX_AUDIO_DEVICES], int num_audio_devices);
int stream_callback(const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data);
void client_data_start_callback();
void client_data_chunk_callback(void *data, size_t size);
void client_data_end_callback();

int main(char argc, const char *argv[])
{
    audio_device_t audio_devices[MAX_AUDIO_DEVICES];
    int num_audio_devices;
    audio_device_t *selected_device;
    ns_server_t server;

    num_audio_devices = audio_init(audio_devices);

    if (num_audio_devices < 0)
    {
        fprintf(stderr, "Error: audio_init() failed\n");
        return EXIT_FAILURE;
    }

    if (num_audio_devices == 0)
    {
        fprintf(stderr, "No audio devices found\n");
        return EXIT_FAILURE;
    }

    selected_device = parse_args(argc, argv, audio_devices, num_audio_devices);
    if (selected_device == NULL)
    {
        return EXIT_FAILURE;
    }

    ds_linked_buffer_init(&client_data_buffer);

    if (ns_server_init(&server, selected_device->port))
    {
        fprintf(stderr, "Error: server_init() failed\n");
        return EXIT_FAILURE;
    }

    if (audio_open(selected_device, &stream_callback, (void *)&server))
    {
        fprintf(stderr, "Error: audio_open() failed\n");
        return EXIT_FAILURE;
    }

    ns_server_set_callbacks(&server, &client_data_start_callback, &client_data_chunk_callback, &client_data_end_callback);
    ns_server_start(&server);

    fprintf(stderr, "Server started\n");

    ns_server_wait(&server);

    if (audio_close(selected_device))
    {
        fprintf(stderr, "Error: audio_close() failed\n");
        return EXIT_FAILURE;
    }

    if (audio_destroy(audio_devices, num_audio_devices))
    {
        fprintf(stderr, "Error: audio_destroy() failed\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

audio_device_t *parse_args(int argc, const char *argv[], audio_device_t audio_devices[MAX_AUDIO_DEVICES], int num_audio_devices)
{
    int sample_rate;
    int tcp_port;
    const char *cmp_result;
    int i, j;

    if (argc != ARGS_EXPECTED)
    {
        fprintf(stderr, "\nUsage: %s (DEVICE_NAME) (SAMPLE_RATE) (TCP_PORT)\n", argv[0]);
        fprintf(stderr, "\tDEVICE_NAME: Name of the sound device\n");
        fprintf(stderr, "\tSAMPLE_RATE: Sample rate to use\n");
        fprintf(stderr, "\tTCP_PORT: Port to stream audio data\n");

        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, "\t%s pulse 44100 8123\n", argv[0]);

        fprintf(stderr, "\nAvailable devices:\n");
        for (i = 0; i < num_audio_devices; i++)
        {
            fprintf(stderr, "\t%s\n", audio_devices[i].name);
        }

        return NULL;
    }

    sample_rate = atoi(argv[ARG_SAMPLE_RATE]);
    tcp_port = atoi(argv[ARG_TCP_PORT]);

    if (sample_rate <= 0)
    {
        fprintf(stderr, "Error: Invalid sample rate: %d, must be positive\n", sample_rate);
        return NULL;
    }

    if (tcp_port <= 0)
    {
        fprintf(stderr, "Error: Invalid TCP port: %d, must be positive\n", tcp_port);
        return NULL;
    }

    for (j = 0; j < num_audio_devices; j++)
    {
        cmp_result = util_strcasestr(audio_devices[j].name, argv[ARG_DEVICE_NAME]);
        if (cmp_result != NULL)
        {
            audio_devices[j].sample_rate = sample_rate;
            audio_devices[j].port = tcp_port;

            return &audio_devices[j];
        }
    }

    fprintf(stderr, "Error: Invalid device name: %s\n", argv[ARG_DEVICE_NAME]);
    return NULL;
}

int stream_callback(const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data)
{
    ns_server_t *server;
    int frames_read;

    server = (ns_server_t *)user_data;

    // Normalize time so that 0.0 approximately corresponds to program start
    if (start_time < 0.0)
        start_time = time_info->currentTime;

    // Send microphone input to clients
    ns_server_broadcast(server, input_buffer, frames_per_buffer * sizeof(float));

    // Send data received from clients to speaker output
    pthread_mutex_lock(&client_data_mutex);
    frames_read = ds_linked_buffer_pop(&client_data_buffer, output_buffer, frames_per_buffer * sizeof(float)) / sizeof(float);
    pthread_mutex_unlock(&client_data_mutex);

    // Fill remainder of buffer (if any) with silence
    while (frames_read < frames_per_buffer)
        ((float *)output_buffer)[frames_read++] = 0.0f;

    return paContinue;
}

void client_data_start_callback()
{
    pthread_mutex_lock(&client_data_mutex);
}

void client_data_chunk_callback(void *data, size_t size)
{
    ds_linked_buffer_push(&client_data_buffer, data, size);
}

void client_data_end_callback()
{
    pthread_mutex_unlock(&client_data_mutex);
}