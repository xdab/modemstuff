#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <configstuff/config.h>
#include <netstuff/netclient.h>
#include <netstuff/netserver.h>
#include <hamstuff/kiss.h>

#include "modulator.h"
#include "demodulator.h"

// #define MOCK_AUDIO_INPUT "tnctest02.raw"
// #define MOCK_BEACON

#define ARGS_USED (1)
#define ARGS_EXPECTED (1 + ARGS_USED)
#define ARG_CONFIG 1

audmod_modulator_t modulator;
audmod_demodulator_t demodulator;

ns_client_t audio_client;
ns_server_t kiss_server;
hs_kiss_decoder_t kiss_decoder;
pthread_mutex_t kiss_decoder_mutex = PTHREAD_MUTEX_INITIALIZER;

void data_callback(void *data, uint32_t length);
void demodulated_frame_callback(hs_ax25_frame_t *frame);
void modulated_samples_callback(ms_float *samples, int samples_count);

void kiss_data_start_callback();
void kiss_data_callback(void *data, uint32_t length);
void kiss_data_end_callback();
void incoming_kiss_message_callback(hs_kiss_message_t *message);

int main(int argc, const char *argv[])
{
    cs_config_t config;
    int sample_rate, port, kiss_port;
    ms_float mark_freq, space_freq, baud_rate;
    const char *host;

    if (argc != ARGS_EXPECTED)
    {
        fprintf(stderr, "\nUsage: %s (CONFIG)\n", argv[0]);
        fprintf(stderr, "\tCONFIG: configuration file\n");

        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, "\t%s packet1200.conf\n", argv[0]);

        return EXIT_FAILURE;
    }

    if (cs_config_read(&config, argv[ARG_CONFIG]))
    {
        fprintf(stderr, "Error: cs_config_read() failed\n");
        return EXIT_FAILURE;
    }

    host = cs_config_get_or_exit(&config, "HOST");
    port = atoi(cs_config_get_or_exit(&config, "PORT"));
    sample_rate = atoi(cs_config_get_or_exit(&config, "RATE"));
    mark_freq = atof(cs_config_get_or_exit(&config, "MARK"));
    space_freq = atof(cs_config_get_or_exit(&config, "SPACE"));
    baud_rate = atof(cs_config_get_or_exit(&config, "BAUD"));
    kiss_port = atoi(cs_config_get_or_exit(&config, "KISSPORT"));

    // Initialize demodulator
    if (audmod_demodulator_init(&demodulator, mark_freq, space_freq, baud_rate, sample_rate))
    {
        fprintf(stderr, "Error: audmod_demodulator_init() failed\n");
        return EXIT_FAILURE;
    }
    audmod_demodulator_set_callbacks(&demodulator, &demodulated_frame_callback);

    // Initialize modulator
    audmod_modulator_init(&modulator, mark_freq, space_freq, baud_rate, sample_rate);
    audmod_modulator_set_callbacks(&modulator, &modulated_samples_callback);

#ifndef MOCK_AUDIO_INPUT

    // Initialize KISS server
    hs_kiss_decoder_init(&kiss_decoder, &incoming_kiss_message_callback);
    if (ns_server_init(&kiss_server, kiss_port))
    {
        fprintf(stderr, "Error: ns_server_init() failed\n");
        return EXIT_FAILURE;
    }
    ns_server_set_callbacks(&kiss_server, &kiss_data_start_callback, &kiss_data_callback, &kiss_data_end_callback);

    // Initialize audio server
    if (ns_client_init(&audio_client))
    {
        fprintf(stderr, "Error: ns_client_init() failed\n");
        return EXIT_FAILURE;
    }
    ns_client_set_callbacks(&audio_client, &data_callback);

    // Connect to the audio server
    if (ns_client_connect(&audio_client, host, port))
    {
        fprintf(stderr, "Error: ns_client_connect() failed\n");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "Connected to %s:%d\n", host, port);

    // Start KISS server
    ns_server_start(&kiss_server);
    fprintf(stderr, "KISS server started\n");

#ifdef MOCK_BEACON
    char buf[512];
    hs_ax25_frame_t frame;
    hs_ax25_frame_init(&frame);

    hs_ax25_call_from_str(&frame.dest, "APRS20");
    hs_ax25_call_from_str(&frame.src, "AB1CDE");

    frame.control = 0x03;
    frame.protocol = 0xf0;

    int frame_number = 0;

    sleep(1);
    while (1)
    {
        frame.info_len = sprintf(frame.info, "!5225.  N/02044.  E Beacon %d", frame_number++);
        audmod_modulator_process(&modulator, &frame);

        hs_ax25_frame_pack_tnc2(&frame, buf);
        fprintf(stderr, "\t%d\tTX\t%s \n", frame_number, buf);
        fflush(stderr);

        sleep(2);
    }
#endif

    // Wait for KISS server thread
    ns_server_wait(&kiss_server);

    fprintf(stderr, "Exiting...\n");

    // Cleanup
    ns_client_destroy(&audio_client);
    ns_server_destroy(&kiss_server);

#else

    // Read audio samples from file
    FILE *file = fopen(MOCK_AUDIO_INPUT, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Error: fopen() failed\n");
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    const int sample_batch_size = 1024;
    long sample_count = file_size / sizeof(ms_float);
    for (long i = 0; i < sample_count; i += sample_batch_size)
    {
        ms_float samples[sample_batch_size];
        fread(samples, sizeof(ms_float), sample_batch_size, file);
        data_callback(samples, sample_batch_size * sizeof(ms_float));
    }

    fclose(file);

#endif

    // Cleanup
    audmod_demodulator_destroy(&demodulator);
    audmod_modulator_destroy(&modulator);

    return EXIT_SUCCESS;
}

void data_callback(void *data, uint32_t length)
{
    audmod_demodulator_process(&demodulator, (ms_float *)data, length / sizeof(ms_float));
}

int frame_count = 0;

void demodulated_frame_callback(hs_ax25_frame_t *frame)
{
    char buf[512];
    hs_kiss_message_t kiss_message;
    int kiss_length;

    // Print the packet to stderr
    hs_ax25_frame_pack_tnc2(frame, buf);
    fprintf(stderr, "\t%d\tRX\t%s \n", ++frame_count, buf);
    fflush(stderr);

    // Send the packet to the KISS clients
    kiss_message.port = 0;
    kiss_message.command = KISS_DATA_FRAME;
    kiss_message.data_length = hs_ax25_frame_pack(frame, kiss_message.data);
    kiss_length = hs_kiss_encode(&kiss_message, buf);
    ns_server_broadcast(&kiss_server, buf, kiss_length);
}

void modulated_samples_callback(ms_float *samples, int samples_count)
{
    ns_client_send(&audio_client, (void *)samples, samples_count * sizeof(ms_float));
}

void kiss_data_start_callback()
{
    pthread_mutex_lock(&kiss_decoder_mutex);
}

void kiss_data_callback(void *data, uint32_t length)
{
    int i;

    for (i = 0; i < length; i++)
        hs_kiss_decoder_process(&kiss_decoder, ((hs_byte *)data)[i]);
}

void kiss_data_end_callback()
{
    pthread_mutex_unlock(&kiss_decoder_mutex);
}

void incoming_kiss_message_callback(hs_kiss_message_t *message)
{
    char buf[512];
    hs_ax25_frame_t frame;

    if (message->command == KISS_DATA_FRAME)
    {
        hs_ax25_frame_unpack(&frame, message->data, message->data_length);

        // Print the packet to stderr
        hs_ax25_frame_pack_tnc2(&frame, buf);
        fprintf(stderr, "TX %s \n", buf);

        // Modulate and send the packet (by modulator's samples callback)
        audmod_modulator_process(&modulator, &frame);
    }
}