#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "config.h"
#include "demodulator.h"

#define ARGS_USED (1)
#define ARGS_EXPECTED (1 + ARGS_USED)
#define ARG_CONFIG 1

#define SAMPLES_BUF_SIZE (1024)

static int received_packets = 0;
static test_demod_demodulator_t demodulator;

void demodulated_packet_callback(hs_ax25_packet_t *packet);

int main(int argc, const char *argv[])
{
    test_demod_config_t config;

    if (argc != ARGS_EXPECTED)
    {
        fprintf(stderr, "\nUsage: %s (CONFIG)\n", argv[0]);
        fprintf(stderr, "\tCONFIG: configuration file\n");

        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, "\t%s packet1200.conf\n", argv[0]);

        return EXIT_FAILURE;
    }

    // Read configuration file
    if (test_demod_config_read(&config, argv[ARG_CONFIG]))
    {
        fprintf(stderr, "Error: test_demod_config_read()\n");
        return EXIT_FAILURE;
    }

    // Initialize demodulator
    if (test_demod_demodulator_init(&demodulator, &config))
    {
        fprintf(stderr, "Error: test_demod_demodulator_init() failed\n");
        return EXIT_FAILURE;
    }
    test_demod_demodulator_set_callbacks(&demodulator, &demodulated_packet_callback);


    // Open the file
    FILE *file = fopen(config.file, "rb");
    if (!file)
    {
        fprintf(stderr, "Error: Unable to open file\n");
        return EXIT_FAILURE;
    }

    // Read the samples in chunks
    ms_float samples[SAMPLES_BUF_SIZE];
    int samples_count;
    while ((samples_count = fread(samples, sizeof(ms_float), SAMPLES_BUF_SIZE, file)) > 0)
        test_demod_demodulator_process(&demodulator, samples, samples_count);

    fprintf(stderr, "Exiting...\n");

    // Cleanup
    test_demod_demodulator_destroy(&demodulator);

    return EXIT_SUCCESS;
}

void demodulated_packet_callback(hs_ax25_packet_t *packet)
{
    char buf[512];
    int kiss_length;
    int ret;

    // Print the packet to stderr
    hs_ax25_packet_pack_tnc2(packet, buf);
    fprintf(stderr, "\t%d\tRX\t%s\n", ++received_packets, buf);
    fflush(stderr);
}
