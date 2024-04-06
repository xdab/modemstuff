#include "buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void buffer_init(buffer_t *buffer)
{
    buffer_node_t *node;

    // Create a buffer with a single, empty node
    node = (buffer_node_t *)malloc(sizeof(buffer_node_t));
    if (node == NULL)
    {
        fprintf(stderr, "Error: malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    node->size = 0;
    node->next = NULL;

    buffer->head = buffer->tail = node;
    buffer->nodes = 1;
}

size_t buffer_size(buffer_t *buffer)
{
    size_t size = 0;
    buffer_node_t *node = buffer->head;

    while (node != NULL)
    {
        size += node->size;
        node = node->next;
    }

    return size;
}

void buffer_push(buffer_t *buffer, void *data, size_t size)
{
    size_t remaining_node_capacity;
    size_t remaining_data_size;
    size_t size_to_store;
    uint8_t *data_ptr;

    // Check for invalid arguments
    if (buffer == NULL)
        return;
    if (data == NULL)
        return;
    if (size == 0)
        return;

    // Initialize variables
    remaining_data_size = size;
    data_ptr = (uint8_t *)data;

    // Find the last node
    buffer_node_t *node = buffer->tail;

    // If the last node is not full, copy data to it until it is
    if ((node->offset + node->size) < BUFFER_NODE_MAX_DATA_SIZE)
    {
        // Calculate how much data can be copied to the node
        remaining_node_capacity = BUFFER_NODE_MAX_DATA_SIZE - (node->offset + node->size);
        size_to_store = (remaining_data_size < remaining_node_capacity) ? remaining_data_size : remaining_node_capacity;

        memcpy(&node->data[node->offset + node->size], data_ptr, size_to_store);
        node->size += size_to_store;
        remaining_data_size -= size_to_store;
        data_ptr += size_to_store;
    }

    // Create new nodes and copy data into them
    while (remaining_data_size > 0)
    {
        node->next = (buffer_node_t *)malloc(sizeof(buffer_node_t));
        if (node->next == NULL)
        {
            fprintf(stderr, "Error: malloc() failed\n");
            exit(EXIT_FAILURE);
        }
        node = node->next;
        node->offset = 0;
        node->next = NULL;

        // Update the buffer metadata
        buffer->tail = node;
        buffer->nodes++;

        remaining_node_capacity = BUFFER_NODE_MAX_DATA_SIZE;
        size_to_store = (remaining_data_size < remaining_node_capacity) ? remaining_data_size : remaining_node_capacity;

        memcpy(node->data, data_ptr, size_to_store);
        node->size = size_to_store;
        remaining_data_size -= size_to_store;
        data_ptr += size_to_store;
    }
}

int buffer_pop(buffer_t *buffer, void *data, size_t size)
{
    uint8_t *data_ptr;
    size_t remaining_data_size;
    size_t size_to_copy;
    size_t output_size;
    buffer_node_t *node;
    buffer_node_t *next_node;

    // Check for invalid arguments
    if (buffer == NULL)
        return -1;
    if (data == NULL)
        return -1;

    // Check for obvious cases
    if (size == 0) // Nothing to pop
        return 0;
    if (buffer->nodes == 1 && buffer->head->size == 0) // Buffer is empty
        return 0;

    // Initialize variables
    data_ptr = (uint8_t *)data;
    remaining_data_size = size;
    output_size = 0;

    // Starting from the head
    node = buffer->head;

    // While there are still nodes in the buffer
    // and not all requested data has been copied
    while (node != NULL && remaining_data_size > 0)
    {
        // Copy as much data as possible or necessary from the current node
        size_to_copy = (node->size < remaining_data_size) ? node->size : remaining_data_size;
        memcpy(data_ptr, node->data + node->offset, size_to_copy);

        // Update pointers and counters to reflect the copied data
        data_ptr += size_to_copy;
        output_size += size_to_copy;
        remaining_data_size -= size_to_copy;

        // Update the current node if it was not fully emptied
        if (size_to_copy < node->size)
        {
            node->size -= size_to_copy;
            node->offset += size_to_copy;
        }
        else // otherwise, remove the node and move to the next one
        {
            next_node = node->next;
            free(node);
            node = next_node;

            // Update the buffer metadata
            buffer->head = next_node;
            buffer->nodes--;
        }
    }

    // At the end, if the buffer contains no nodes, reinitialize it
    // to contain a single empty node
    if (buffer->nodes == 0)
        buffer_init(buffer);

    // Return the actual number of bytes copied
    return output_size;
}