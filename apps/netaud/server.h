#ifndef MODEMSTUFF_APPS_NETAUD_SERVER_H
#define MODEMSTUFF_APPS_NETAUD_SERVER_H

#include <pthread.h>

#define MAX_CLIENTS (32)

typedef struct server
{
    int server_socket;
    int client_sockets[MAX_CLIENTS];
    pthread_mutex_t client_sockets_mutex;
    int run;
    pthread_t server_thread;

    void (*client_data_start_callback)();
    void (*client_data_chunk_callback)(void *, size_t);
    void (*client_data_end_callback)();

} server_t;

int server_init(server_t *server, int port);

void server_set_callbacks(server_t *server, 
    void (*client_data_start_callback)(void), 
    void (*client_data_chunk_callback)(void *, size_t), 
    void (*client_data_end_callback)(void));

void server_start(server_t *server);

void server_wait(server_t *server);

void server_broadcast(server_t *server, const void *data, size_t size);

void server_destroy(server_t *server);

#endif // MODEMSTUFF_APPS_NETAUD_SERVER_H
