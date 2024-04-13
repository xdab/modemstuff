#ifndef _NETSTUFF_NETSERVER_H
#define _NETSTUFF_NETSERVER_H

#include <pthread.h>
#include <stdint.h>

#define MAX_CLIENTS (32)

typedef struct ns_server
{
    int server_socket;
    int client_sockets[MAX_CLIENTS];
    pthread_mutex_t client_sockets_mutex;

    // Thread to handle incoming connections and data
    pthread_t server_thread;
    int run;

    // Data callbacks
    void (*client_data_start_callback)();
    void (*client_data_chunk_callback)(void *, uint32_t);
    void (*client_data_end_callback)();

} ns_server_t;

int ns_server_init(ns_server_t *server, uint16_t port);

void ns_server_set_callbacks(ns_server_t *server,
                             void (*client_data_start_callback)(void),
                             void (*client_data_chunk_callback)(void *, uint32_t),
                             void (*client_data_end_callback)(void));

void ns_server_start(ns_server_t *server);

void ns_server_wait(ns_server_t *server);

void ns_server_broadcast(ns_server_t *server, const void *data, uint32_t size);

void ns_server_destroy(ns_server_t *server);

#endif // _NETSTUFF_NETSERVER_H