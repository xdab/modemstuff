#include <netstuff/netserver.h>

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <pthread.h>

#define CONNECTION_QUEUE_SIZE (5)
#define DATA_BUF_SIZE (1024)

void *server_thread_function(void *data);

int ns_server_init(ns_server_t *server, uint16_t port)
{
    struct sockaddr_in server_address;

    // Clear and initialize the server structure
    memset(server, 0, sizeof(ns_server_t));
    server->client_sockets_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    // Clear and initialize the server address structure
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    // Create a new server socket
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_socket < 0)
    {
        fprintf(stderr, "Error: socket() failed\n");
        return -1;
    }

    // Allow the server socket to be reused immediately after it is closed
    int reuse = 1;
    if (setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        fprintf(stderr, "Error: setsockopt() failed\n");
        close(server->server_socket);
        return -1;
    }

    // Bind newly created socket to given address
    if (bind(server->server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        fprintf(stderr, "Error: bind() failed\n");
        close(server->server_socket);
        return -1;
    }

    // Listen for connections, but do not accept them yet
    if (listen(server->server_socket, CONNECTION_QUEUE_SIZE) < 0)
    {
        fprintf(stderr, "Error: listen() failed\n");
        close(server->server_socket);
        return -1;
    }

    return 0;
}

void ns_server_set_callbacks(ns_server_t *server,
                          void (*client_data_start_callback)(void),
                          void (*client_data_chunk_callback)(void *, uint32_t),
                          void (*client_data_end_callback)(void))
{
    server->client_data_start_callback = client_data_start_callback;
    server->client_data_chunk_callback = client_data_chunk_callback;
    server->client_data_end_callback = client_data_end_callback;
}

void ns_server_start(ns_server_t *server)
{
    server->run = 1;

    if (pthread_create(&server->server_thread, NULL, &server_thread_function, (void *)server))
    {
        fprintf(stderr, "Error: pthread_create() for server thread failed\n");
        exit(EXIT_FAILURE);
    }
}

void ns_server_wait(ns_server_t *server)
{
    pthread_join(server->server_thread, NULL);
}

void ns_server_broadcast(ns_server_t *server, const void *data, uint32_t size)
{
    int i, sent;

    if (size <= 0)
        return;

    if (data == NULL)
        return;

    for (i = 0; i < MAX_CLIENTS; i++)
        if (server->client_sockets[i] > 0)
        {
            sent = send(server->client_sockets[i], data, size, 0);
            if (sent < 0)
            {
                // Lock the client_sockets array so a closed socket is not polled
                // by the pselect() call in server_thread_function()
                pthread_mutex_lock(&server->client_sockets_mutex);

                fprintf(stderr, "Client at slot %d disconnected\n", i);
                close(server->client_sockets[i]);
                server->client_sockets[i] = 0;

                // Unlock the client_sockets array just after the socket is closed
                // and marked as such with 0
                pthread_mutex_unlock(&server->client_sockets_mutex);
            }
        }
}

void ns_server_destroy(ns_server_t *server)
{
    int i;

    pthread_cancel(server->server_thread);
    close(server->server_socket);
    for (i = 0; i < MAX_CLIENTS; i++)
        if (server->client_sockets[i] > 0)
            close(server->client_sockets[i]);
}

void *server_thread_function(void *data)
{
    int i;
    int pselect_result;
    fd_set sockets_to_read;
    int max_socket;
    struct timespec timespec;
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_length;
    char client_address_str[INET_ADDRSTRLEN];
    char client_data_buf[DATA_BUF_SIZE];
    int client_data_chunk_len;
    int client_data_total_len;
    ns_server_t *server;

    server = (ns_server_t *)data;

    while (server->run)
    {
        FD_ZERO(&sockets_to_read);

        timespec.tv_sec = 1;
        timespec.tv_nsec = 0;

        // Wait for read possibility on any of the sockets
        max_socket = server->server_socket;
        FD_SET(server->server_socket, &sockets_to_read);

        // If there is a client data callback, add all client sockets to the set
        // otherwise, do not bother
        if (server->client_data_chunk_callback != NULL)
        {
            // Lock the client_sockets array so no client connections are closed
            // by disconnection detection in server_broadcast()
            pthread_mutex_lock(&server->client_sockets_mutex);

            for (i = 0; i < MAX_CLIENTS; i++)
                if (server->client_sockets[i] > 0)
                {
                    FD_SET(server->client_sockets[i], &sockets_to_read);
                    if (server->client_sockets[i] > max_socket)
                        max_socket = server->client_sockets[i];
                }

            // Unlock the client_sockets array just before pselect()
            pthread_mutex_unlock(&server->client_sockets_mutex);
        }

        pselect_result = pselect(max_socket + 1, &sockets_to_read, NULL, NULL, &timespec, NULL);
        if (pselect_result < 0)
        {
            fprintf(stderr, "Error: pselect() failed\n");
            break;
        }

        if (pselect_result == 0)
        {
            // Timeout
            continue;
        }

        // Activity on the server socket - new connection
        if (FD_ISSET(server->server_socket, &sockets_to_read))
        {
            // Accept incoming connection
            client_socket = accept(server->server_socket, (struct sockaddr *)&client_address, &client_address_length);
            if (client_socket < 0)
            {
                fprintf(stderr, "Error: accept() failed\n");
                break;
            }

            inet_ntop(AF_INET, &client_address.sin_addr, client_address_str, INET_ADDRSTRLEN);
            fprintf(stderr, "Client connected from %s ", client_address_str);

            // Find an empty slot in the client_sockets array
            for (i = 0; i < MAX_CLIENTS; i++)
                if (server->client_sockets[i] <= 0)
                {
                    server->client_sockets[i] = client_socket;
                    fprintf(stderr, "and assigned slot %d, socket %d\n", i, client_socket);
                    break;
                }

            // Reject the connection if there are no empty slots
            if (i == MAX_CLIENTS)
            {
                fprintf(stderr, "and rejected (no empty slots)\n");
                close(client_socket);
            }
        }

        // If there is a client data callback, find the client sockets with activity
        // read the incoming data and call the client data callback
        // otherwise, do not bother
        if (server->client_data_chunk_callback != NULL)
        {
            for (i = 0; i < MAX_CLIENTS; i++)
                if ((server->client_sockets[i] > 0) && (FD_ISSET(server->client_sockets[i], &sockets_to_read)))
                {
                    client_data_total_len = 0;

                    do
                    {
                        // Read a chunk of data from the client socket
                        client_data_chunk_len = recv(server->client_sockets[i], client_data_buf, DATA_BUF_SIZE, 0);
                        if (client_data_chunk_len < 0)
                        {
                            // Here there is no need to lock the client_sockets array
                            // because its in the same thread as the pselect() call
                            fprintf(stderr, "Client at slot %d disconnected\n", i);
                            close(server->client_sockets[i]);
                            server->client_sockets[i] = 0;
                            break;
                        }

                        if (client_data_chunk_len > 0)
                        {
                            if (client_data_total_len == 0 && server->client_data_start_callback != NULL)
                            {
                                // Call the client data start callback before the first chunk of data
                                server->client_data_start_callback();
                            }

                            // Call the client data callback for the received data (or a part of it)
                            server->client_data_chunk_callback(client_data_buf, client_data_chunk_len);

                            // Update the total length of data received
                            client_data_total_len += client_data_chunk_len;
                        }

                        // Repeat until the chunk is smaller than buffer size
                    } while (client_data_chunk_len == DATA_BUF_SIZE);

                    // Call the client data end callback after all (and not zero) data is received
                    if (client_data_total_len > 0 && server->client_data_end_callback != NULL)
                        server->client_data_end_callback();
                }
        }
    }

    return NULL;
}
