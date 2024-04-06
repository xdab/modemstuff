#ifndef MODEMSTUFF_APPS_NETAUD_BUFFER_H
#define MODEMSTUFF_APPS_NETAUD_BUFFER_H

#include <stddef.h>
#include <stdint.h>

#define BUFFER_NODE_MAX_DATA_SIZE 1024

typedef struct buffer_node
{
    uint8_t data[BUFFER_NODE_MAX_DATA_SIZE];
    uint16_t size;
    uint16_t offset;
    struct buffer_node *next;
} buffer_node_t;

typedef struct buffer
{
    buffer_node_t *head, *tail;
    size_t nodes;

} buffer_t;

void buffer_init(buffer_t *buffer);

size_t buffer_size(buffer_t *buffer);

void buffer_push(buffer_t *buffer, void *data, size_t size);

int buffer_pop(buffer_t *buffer, void *data, size_t size);

#endif // MODEMSTUFF_APPS_NETAUD_BUFFER_H
