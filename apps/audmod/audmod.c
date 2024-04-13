#include <stdio.h>
#include <stdlib.h>

#include <netstuff/netclient.h>
#include <modemstuff/fsk.h>
#include <modemstuff/bitdet.h>
#include <modemstuff/linecode.h>
#include <modemstuff/ax25_deframer.h>

#define ARGS_USED (6)
#define ARGS_EXPECTED (1 + ARGS_USED)
#define ARG_HOST 1
#define ARG_PORT 2
#define ARG_SAMPLE_RATE 3
#define ARG_MARK_FREQ 4
#define ARG_SPACE_FREQ 5
#define ARG_BAUD_RATE 6

ms_fsk_detector_t fsk_detector;
ms_bit_detector_t bit_detector;
ms_linecode_nrzi_decoder_t nrzi_decoder;
ms_ax25_deframer_t ax25_deframer;

void data_callback(void *data, uint32_t length);

int main(int argc, const char *argv[])
{
    ns_client_t client;
    int sample_rate, port;
    ms_float mark_freq, space_freq, baud_rate;
    const char *host;

    if (argc != ARGS_EXPECTED)
    {
        fprintf(stderr, "\nUsage: %s (HOST) (PORT) (SAMPLE_RATE) (MARK_FREQ) (SPACE_FREQ) (BAUD_RATE)\n", argv[0]);
        fprintf(stderr, "\tHOST: Host to connect to\n");
        fprintf(stderr, "\tPORT: Port to connect to\n");
        fprintf(stderr, "\tSAMPLE_RATE: Sample rate to use\n");

        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, "\t%s 192.168.50.1 8800 44100 1200 2200 1200\n", argv[0]);

        return EXIT_FAILURE;
    }

    host = argv[ARG_HOST];
    port = atoi(argv[ARG_PORT]);
    sample_rate = atoi(argv[ARG_SAMPLE_RATE]);
    mark_freq = atof(argv[ARG_MARK_FREQ]);
    space_freq = atof(argv[ARG_SPACE_FREQ]);
    baud_rate = atof(argv[ARG_BAUD_RATE]);

    if (ms_fsk_detector_init(&fsk_detector, mark_freq, space_freq, baud_rate, sample_rate))
    {
        fprintf(stderr, "Error: ms_fsk_detector_init() failed\n");
        return EXIT_FAILURE;
    }

    ms_bit_detector_init(&bit_detector, sample_rate, baud_rate);
    ms_linecode_nrzi_decoder_init(&nrzi_decoder);
    ms_ax25_deframer_init(&ax25_deframer);

    if (ns_client_init(&client))
    {
        fprintf(stderr, "Error: ns_client_init() failed\n");
        return EXIT_FAILURE;
    }

    ns_client_set_callbacks(&client, &data_callback);

    if (ns_client_connect(&client, host, port))
    {
        fprintf(stderr, "Error: ns_client_connect() failed\n");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Connected to %s:%d\n", host, port);
    getchar();

    ms_fsk_detector_destroy(&fsk_detector);
    ns_client_destroy(&client);

    return EXIT_SUCCESS;
}

void data_callback(void *data, uint32_t length)
{
    uint32_t i;
    float sample;
    ms_float result;
    ms_bit bit;
    ms_ax25_frame_t frame;
    char packet_str[512];

    for (i = 0; i < length; i += sizeof(float))
    {
        sample = *((float *)(data + i));
        result = ms_fsk_detector_process(&fsk_detector, sample);
        bit = ms_bit_detector_process(&bit_detector, result);
        bit = ms_linecode_nrzi_decode(&nrzi_decoder, bit);

        if ((bit != MS_BIT_NONE) && ms_ax25_deframer_process(&ax25_deframer, &frame, bit))
        {
            ms_ax25_frame_pack_tnc2(&frame, packet_str);
            fputs(packet_str, stderr);
            fputs("\r\n\n", stderr);
        }
    }
}
