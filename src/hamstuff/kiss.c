#include <hamstuff/kiss.h>

#define KISS_END 0xC0
#define KISS_ESC 0xDB
#define KISS_ESC_END 0xDC
#define KISS_ESC_ESC 0xDD

#define KISS_MIN_FRAME_SIZE 1

void _hs_kiss_decoder_process_frame(hs_kiss_decoder_t *decoder);

void hs_kiss_decoder_init(hs_kiss_decoder_t *decoder, void (*frame_callback)(hs_kiss_message_t *message))
{
    decoder->state = KISS_STATE_UNDEFINED;
    decoder->buffer_pos = 0;
    decoder->frame_callback = frame_callback;
}

void hs_kiss_decoder_process(hs_kiss_decoder_t *decoder, hs_byte byte)
{
    switch (decoder->state)
    {
    case KISS_STATE_UNDEFINED:
        if (byte == KISS_END)
        {
            // Begin reading frame
            decoder->state = KISS_STATE_READING;
            decoder->buffer_pos = 0;
        }
        break;
    case KISS_STATE_READING:
        if (byte == KISS_ESC)
        {
            // Begin reading escaped byte
            decoder->state = KISS_STATE_READING_ESCAPED;
        }
        else if (byte == KISS_END)
        {
            // Handle apparent end of frame
            if (decoder->frame_callback && decoder->buffer_pos >= KISS_MIN_FRAME_SIZE)
            {
                // Process frame
                _hs_kiss_decoder_process_frame(decoder);
            }

            // Begin reading next frame
            decoder->buffer_pos = 0;
            decoder->state = KISS_STATE_READING;
        }
        else if (decoder->buffer_pos < HS_KISS_DECODER_BUFFER_SIZE)
        {
            // Copy byte to buffer
            decoder->buffer[decoder->buffer_pos++] = byte;
        }
        else
        {
            // Buffer would overflow, reset
            decoder->state = KISS_STATE_UNDEFINED;
            decoder->buffer_pos = 0;
        }
        break;
    case KISS_STATE_READING_ESCAPED:
        if (decoder->buffer_pos < HS_KISS_DECODER_BUFFER_SIZE)
        {
            // Un-escape byte
            if (byte == KISS_ESC_END)
                decoder->buffer[decoder->buffer_pos++] = KISS_END;
            else if (byte == KISS_ESC_ESC)
                decoder->buffer[decoder->buffer_pos++] = KISS_ESC;

            // Resume reading frame
            decoder->state = KISS_STATE_READING;
        }
        else
        {
            // Buffer would overflow, reset
            decoder->state = KISS_STATE_UNDEFINED;
            decoder->buffer_pos = 0;
        }
        break;
    }
}

void _hs_kiss_decoder_process_frame(hs_kiss_decoder_t *decoder)
{
    hs_kiss_message_t message;
    int i;

    // Interpret (port, command)
    if (decoder->buffer[0] == 0xff) // Special case for KISS_RETURN
    {
        message.port = 0;
        message.command = KISS_RETURN;
        message.data_length = 0;
    }
    else // Normal interpretation
    {
        message.port = (decoder->buffer[0] >> 4) & 0x0f;
        message.command = decoder->buffer[0] & 0x0f;
        message.data_length = decoder->buffer_pos - 1;
    }

    // Copy data
    for (i = 0; i < message.data_length; i++)
        message.data[i] = decoder->buffer[i + 1];

    // Call callback
    decoder->frame_callback(&message);
}

int hs_kiss_encode(hs_kiss_message_t *message, hs_byte *buffer)
{
    int i;
    int pos = 0;

    // Append start of frame
    buffer[pos++] = KISS_END;

    // Encode (port, command)
    buffer[pos++] = (message->port << 4) | message->command;

    // Copy data
    for (i = 0; i < message->data_length; i++)
    {
        if (message->data[i] == KISS_END)
        {
            buffer[pos++] = KISS_ESC;
            buffer[pos++] = KISS_ESC_END;
        }
        else if (message->data[i] == KISS_ESC)
        {
            buffer[pos++] = KISS_ESC;
            buffer[pos++] = KISS_ESC_ESC;
        }
        else
        {
            buffer[pos++] = message->data[i];
        }
    }

    // Append end of frame
    buffer[pos++] = KISS_END;

    return pos;
}