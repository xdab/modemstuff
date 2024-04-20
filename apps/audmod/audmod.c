#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <netstuff/netclient.h>
#include <netstuff/netserver.h>
#include <hamstuff/kiss.h>

#include "config.h"
#include "modulator.h"
#include "demodulator.h"

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
    audmod_config_t config;

    if (argc != ARGS_EXPECTED)
    {
        fprintf(stderr, "\nUsage: %s (CONFIG)\n", argv[0]);
        fprintf(stderr, "\tCONFIG: configuration file\n");

        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, "\t%s packet1200.conf\n", argv[0]);

        return EXIT_FAILURE;
    }

    // Read configuration file
    if (audmod_config_read(&config, argv[ARG_CONFIG]))
    {
        fprintf(stderr, "Error: audmod_config_read()\n");
        return EXIT_FAILURE;
    }

    // Initialize demodulator
    if (audmod_demodulator_init(&demodulator, &config))
    {
        fprintf(stderr, "Error: audmod_demodulator_init() failed\n");
        return EXIT_FAILURE;
    }
    audmod_demodulator_set_callbacks(&demodulator, &demodulated_frame_callback);

    // Initialize modulator
    audmod_modulator_init(&modulator, &config);
    audmod_modulator_set_callbacks(&modulator, &modulated_samples_callback);

    // Initialize KISS server
    hs_kiss_decoder_init(&kiss_decoder, &incoming_kiss_message_callback);
    if (ns_server_init(&kiss_server, config.kiss_port))
    {
        fprintf(stderr, "Error: ns_server_init() failed\n");
        return EXIT_FAILURE;
    }
    ns_server_set_callbacks(&kiss_server, &kiss_data_start_callback, &kiss_data_callback, &kiss_data_end_callback);

    // Start KISS server
    ns_server_start(&kiss_server);

    // Initialize audio server
    if (ns_client_init(&audio_client))
    {
        fprintf(stderr, "Error: ns_client_init() failed\n");
        return EXIT_FAILURE;
    }
    ns_client_set_callbacks(&audio_client, &data_callback);

    // Connect to the audio server
    if (ns_client_connect(&audio_client, config.host, config.port))
    {
        fprintf(stderr, "Error: ns_client_connect() failed\n");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "Connected to %s:%d\n", config.host, config.port);

    // Wait for KISS server thread
    ns_server_wait(&kiss_server);

    fprintf(stderr, "Exiting...\n");

    // Cleanup
    ns_client_destroy(&audio_client);
    ns_server_destroy(&kiss_server);
    audmod_demodulator_destroy(&demodulator);
    audmod_modulator_destroy(&modulator);

    return EXIT_SUCCESS;
}

void data_callback(void *data, uint32_t length)
{
    // Pass the samples to the demodulator
    audmod_demodulator_process(&demodulator, (ms_float *)data, length / sizeof(ms_float));
}

void demodulated_frame_callback(hs_ax25_frame_t *frame)
{
    char buf[512];
    hs_kiss_message_t kiss_message;
    int kiss_length;

    // Print the packet to stderr
    hs_ax25_frame_pack_tnc2(frame, buf);
    fprintf(stderr, "\tRX\t%s\n", buf);
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
    // Send samples to the audio server
    ns_client_send(&audio_client, (void *)samples, samples_count * sizeof(ms_float));
}

void kiss_data_start_callback()
{
    // Lock the KISS decoder so that messages from other KISS clients do not mix
    pthread_mutex_lock(&kiss_decoder_mutex);
}

void kiss_data_callback(void *data, uint32_t length)
{
    int i;

    for (i = 0; i < length; i++)
        // Pass the bytes to the KISS decoder
        hs_kiss_decoder_process(&kiss_decoder, ((hs_byte *)data)[i]);
}

void kiss_data_end_callback()
{
    // Unlock the KISS decoder
    pthread_mutex_unlock(&kiss_decoder_mutex);
}

void incoming_kiss_message_callback(hs_kiss_message_t *message)
{
    char buf[512];
    hs_ax25_frame_t frame;

    switch (message->command)
    {
    case KISS_DATA_FRAME:
        hs_ax25_frame_unpack(&frame, message->data, message->data_length);

        // Print the packet to stderr
        hs_ax25_frame_pack_tnc2(&frame, buf);
        fprintf(stderr, "\tTX\t%s\n", buf);

        // Modulate and send the packet (by modulator's samples callback)
        audmod_modulator_process(&modulator, &frame);
        break;

    case KISS_TX_DELAY:
        // Set the TX delay in milliseconds instead of 10ms units
        modulator.tx_delay = message->data[0] * 10;

        // Print info to stderr
        fprintf(stderr, "\tTXDELAY\t%d ms\n", modulator.tx_delay);
        break;

    case KISS_TX_TAIL:
        // Set the TX tail in milliseconds instead of 10ms units
        modulator.tx_tail = message->data[0] * 10;

        // Print info to stderr
        fprintf(stderr, "\tTXTAIL\t%d ms\n", modulator.tx_tail);
        break;

    default:
        fprintf(stderr, "Unsupported KISS command: 0x%02x\n", message->command);
        break;
    }
}