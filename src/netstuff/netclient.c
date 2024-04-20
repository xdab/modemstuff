#include <netstuff/netclient.h>

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

#define DATA_BUF_SIZE (512)

void *client_thread_function(void *data);

int ns_client_init(ns_client_t *client)
{
    // Clear the client structure
    memset(client, 0, sizeof(ns_client_t));

    return 0;
}

void ns_client_set_callbacks(ns_client_t *client, void (*data_callback)(void *, uint32_t))
{
    client->data_callback = data_callback;
}

int ns_client_connect(ns_client_t *client, const char *host, uint16_t port)
{
    struct sockaddr_in server_address;
    struct hostent *host_db_entry;

    // Clear and initialize the server address structure
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    // Find server host in host database
    host_db_entry = gethostbyname(host);
    if (host_db_entry == NULL)
    {
        fprintf(stderr, "Error: gethostbyname() failed\n");
        return -1;
    }

    // Extract the server's IP address from the host database entry
    memcpy(&server_address.sin_addr.s_addr, host_db_entry->h_addr_list[0], host_db_entry->h_length);

    // Create a new client socket
    client->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socket < 0)
    {
        fprintf(stderr, "Error: socket() failed\n");
        return -1;
    }

    // Connect to the server
    if (connect(client->socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        fprintf(stderr, "Error: connect() failed\n");
        close(client->socket);
        return -1;
    }

    // Create and start a thread to receive data
    client->run = 1;
    if (pthread_create(&client->thread, NULL, &client_thread_function, (void *)client))
    {
        fprintf(stderr, "Error: pthread_create() failed\n");
        close(client->socket);
        return -1;
    }

    return 0;
}

int ns_client_send(ns_client_t *client, const void *data, uint32_t size)
{
    int bytes_sent;

    // Send the data to the server
    bytes_sent = send(client->socket, data, size, 0);
    if (bytes_sent < 0)
    {
        fprintf(stderr, "Error: send() failed\n");
        return -1;
    }

    return 0;
}

void ns_client_destroy(ns_client_t *client)
{
    // Stop the client thread
    client->run = 0;
    pthread_join(client->thread, NULL);

    // Close the client socket
    close(client->socket);
}

void *client_thread_function(void *data)
{
    ns_client_t *client;
    char buffer[DATA_BUF_SIZE];
    int bytes_received;

    client = (ns_client_t *)data;

    while (client->run)
    {
        bytes_received = recv(client->socket, buffer, sizeof(buffer), 0);
        if (bytes_received < 0)
        {
            fprintf(stderr, "Error: recv() failed\n");
            break;
        }
        else if (bytes_received == 0)
        {
            fprintf(stderr, "Error: connection closed by server\n");
            break;
        }

        if (client->data_callback)
            client->data_callback(buffer, bytes_received);
    }

    return NULL;
}