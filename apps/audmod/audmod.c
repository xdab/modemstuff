#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <configstuff/config.h>
#include <netstuff/netclient.h>
#include <netstuff/netserver.h>
#include <modemstuff/fsk.h>
#include <modemstuff/bitdet.h>
#include <modemstuff/linecode.h>
#include <hamstuff/ax25_deframer.h>
#include <hamstuff/kiss.h>

#define ARGS_USED (1)
#define ARGS_EXPECTED (1 + ARGS_USED)
#define ARG_CONFIG 1

ms_fsk_detector_t fsk_detector;
ms_bit_detector_t bit_detector;
ms_linecode_nrzi_decoder_t nrzi_decoder;
hs_ax25_deframer_t ax25_deframer;

ns_server_t kiss_server;
hs_kiss_decoder_t kiss_decoder;
pthread_mutex_t kiss_decoder_mutex = PTHREAD_MUTEX_INITIALIZER;

void data_callback(void *data, uint32_t length);
void kiss_data_start_callback();
void kiss_data_callback(void *data, uint32_t length);
void kiss_data_end_callback();
void incoming_kiss_message_callback(hs_kiss_message_t *message);

int main(int argc, const char *argv[])
{
    cs_config_t config;
    ns_client_t client;
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

    // Initialize modem and data link layer components
    if (ms_fsk_detector_init(&fsk_detector, mark_freq, space_freq, baud_rate, sample_rate))
    {
        fprintf(stderr, "Error: ms_fsk_detector_init() failed\n");
        return EXIT_FAILURE;
    }
    ms_bit_detector_init(&bit_detector, sample_rate, baud_rate);
    ms_linecode_nrzi_decoder_init(&nrzi_decoder);
    hs_ax25_deframer_init(&ax25_deframer);

    // Initialize audio server
    if (ns_client_init(&client))
    {
        fprintf(stderr, "Error: ns_client_init() failed\n");
        return EXIT_FAILURE;
    }
    ns_client_set_callbacks(&client, &data_callback);

    // Initialize KISS server
    hs_kiss_decoder_init(&kiss_decoder, &incoming_kiss_message_callback);
    if (ns_server_init(&kiss_server, kiss_port))
    {
        fprintf(stderr, "Error: ns_server_init() failed\n");
        return EXIT_FAILURE;
    }
    ns_server_set_callbacks(&kiss_server, &kiss_data_start_callback, &kiss_data_callback, &kiss_data_end_callback);

    // Connect to the audio server
    if (ns_client_connect(&client, host, port))
    {
        fprintf(stderr, "Error: ns_client_connect() failed\n");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "Connected to %s:%d\n", host, port);

    // Start KISS server
    ns_server_start(&kiss_server);
    fprintf(stderr, "KISS server started\n");

    // Wait for KISS server thread
    ns_server_wait(&kiss_server);

    fprintf(stderr, "Exiting...\n");

    // Cleanup
    ms_fsk_detector_destroy(&fsk_detector);
    ns_client_destroy(&client);
    ns_server_destroy(&kiss_server);

    return EXIT_SUCCESS;
}

void data_callback(void *data, uint32_t length)
{
    uint32_t i;
    float sample;
    ms_float result;
    ms_bit bit;
    hs_ax25_frame_t frame;
    char buf[512];
    hs_kiss_message_t kiss_message;
    int kiss_length;

    for (i = 0; i < length; i += sizeof(float))
    {
        sample = *((float *)(data + i));
        result = ms_fsk_detector_process(&fsk_detector, sample);
        bit = ms_bit_detector_process(&bit_detector, result);
        bit = ms_linecode_nrzi_decode(&nrzi_decoder, bit);

        if ((bit != MS_BIT_NONE) && hs_ax25_deframer_process(&ax25_deframer, &frame, bit))
        {
            // Print the packet to stderr
            hs_ax25_frame_pack_tnc2(&frame, buf);
            fprintf(stderr, "RX %s \n", buf);

            // Send the packet to the KISS clients
            kiss_message.port = 0;
            kiss_message.command = KISS_DATA_FRAME;
            kiss_message.data_length = hs_ax25_frame_pack(&frame, kiss_message.data);
            kiss_length = hs_kiss_encode(&kiss_message, buf);
            ns_server_broadcast(&kiss_server, buf, kiss_length);
        }
    }
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
    }
}