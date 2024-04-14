#ifndef _MODEMSTUFF_KISS_H
#define _MODEMSTUFF_KISS_H

#include <modemstuff/modemstuff.h>

#ifndef MS_KISS_DECODER_BUFFER_SIZE
#define MS_KISS_DECODER_BUFFER_SIZE 512
#endif

typedef enum ms_kiss_state
{
    KISS_STATE_UNDEFINED,
    KISS_STATE_READING,
    KISS_STATE_READING_ESCAPED
} ms_kiss_state_t;

typedef enum ms_kiss_command
{
    KISS_DATA_FRAME = 0x00,
    KISS_TX_DELAY = 0x01,
    KISS_P = 0x02,
    KISS_SLOT_TIME = 0x03,
    KISS_TX_TAIL = 0x04,
    KISS_FULL_DUPLEX = 0x05,
    KISS_SET_HARDWARE = 0x06,
    KISS_RETURN = 0xFF
} ms_kiss_command_t;

typedef struct ms_kiss_message
{
    ms_byte port;
    ms_kiss_command_t command;
    ms_byte data[MS_KISS_DECODER_BUFFER_SIZE];
    int data_length;
} ms_kiss_message_t;

typedef struct ms_kiss_decoder
{
    ms_kiss_state_t state;
    ms_byte buffer[MS_KISS_DECODER_BUFFER_SIZE];
    int buffer_pos;
    void (*frame_callback)(ms_kiss_message_t *message);
} ms_kiss_decoder_t;

void ms_kiss_decoder_init(ms_kiss_decoder_t *decoder, void (*frame_callback)(ms_kiss_message_t *message));

void ms_kiss_decoder_process(ms_kiss_decoder_t *decoder, ms_byte byte);

int ms_kiss_encode(ms_kiss_message_t *message, ms_byte *buffer);

#endif // _MODEMSTUFF_KISS_H