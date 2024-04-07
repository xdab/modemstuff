#ifndef _MODEMSTUFF_DATASTUFF_LINKEDBUFFER_H
#define _MODEMSTUFF_DATASTUFF_LINKEDBUFFER_H

#include <stdint.h>

#define DS_LINKED_BUFFER_NODE_CAPACITY 1024

typedef struct ds_linked_buffer_node
{
    uint8_t data[DS_LINKED_BUFFER_NODE_CAPACITY];
    uint16_t size;
    uint16_t offset;
    struct ds_linked_buffer_node *next;
} ds_linked_buffer_node_t;

typedef struct ds_linked_buffer
{
    uint32_t nodes;
    ds_linked_buffer_node_t *head, *tail;
} ds_linked_buffer_t;

void ds_linked_buffer_init(ds_linked_buffer_t *buffer);

uint32_t ds_linked_buffer_size(ds_linked_buffer_t *buffer);

void ds_linked_buffer_push(ds_linked_buffer_t *buffer, void *data, uint32_t size);

int ds_linked_buffer_pop(ds_linked_buffer_t *buffer, void *data, uint32_t size);

#endif // _MODEMSTUFF_DATASTUFF_LINKEDBUFFER_H