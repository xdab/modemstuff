#ifndef _NETSTUFF_NETCLIENT_H
#define _NETSTUFF_NETCLIENT_H

#include <pthread.h>
#include <stdint.h>

typedef struct ns_client
{
    int socket;
    pthread_t thread;
    int run;
    void (*data_callback)(void *, uint32_t);
    
} ns_client_t;

int ns_client_init(ns_client_t *client);

void ns_client_set_callbacks(ns_client_t *client, void (*data_callback)(void *, uint32_t));

int ns_client_connect(ns_client_t *client, const char *host, uint16_t port);

int ns_client_send(ns_client_t *client, const void *data, uint32_t size);

void ns_client_destroy(ns_client_t *client);

#endif // _NETSTUFF_NETCLIENT_H