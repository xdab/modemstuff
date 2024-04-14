#ifndef _HAMSTUFF_KISS_H
#define _HAMSTUFF_KISS_H

#include <hamstuff/hamstuff.h>

#ifndef HS_KISS_DECODER_BUFFER_SIZE
#define HS_KISS_DECODER_BUFFER_SIZE 512
#endif

typedef enum hs_kiss_state
{
    KISS_STATE_UNDEFINED,
    KISS_STATE_READING,
    KISS_STATE_READING_ESCAPED
} hs_kiss_state_t;

typedef enum hs_kiss_command
{
    KISS_DATA_FRAME = 0x00,
    KISS_TX_DELAY = 0x01,
    KISS_P = 0x02,
    KISS_SLOT_TIME = 0x03,
    KISS_TX_TAIL = 0x04,
    KISS_FULL_DUPLEX = 0x05,
    KISS_SET_HARDWARE = 0x06,
    KISS_RETURN = 0xFF
} hs_kiss_command_t;

typedef struct hs_kiss_message
{
    hs_byte port;
    hs_kiss_command_t command;
    hs_byte data[HS_KISS_DECODER_BUFFER_SIZE];
    int data_length;
} hs_kiss_message_t;

typedef struct hs_kiss_decoder
{
    hs_kiss_state_t state;
    hs_byte buffer[HS_KISS_DECODER_BUFFER_SIZE];
    int buffer_pos;
    void (*frame_callback)(hs_kiss_message_t *message);
} hs_kiss_decoder_t;

void hs_kiss_decoder_init(hs_kiss_decoder_t *decoder, void (*frame_callback)(hs_kiss_message_t *message));

void hs_kiss_decoder_process(hs_kiss_decoder_t *decoder, hs_byte byte);

int hs_kiss_encode(hs_kiss_message_t *message, hs_byte *buffer);

#endif // _MODEMSTUFF_KISS_H