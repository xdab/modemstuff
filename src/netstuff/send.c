#include <netstuff/send.h>

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int ns_send(const char *host, int port, const void *data, unsigned int size)
{
    int client_socket;
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
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0)
    {
        fprintf(stderr, "Error: socket() failed\n");
        return -1;
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        fprintf(stderr, "Error: connect() failed\n");
        close(client_socket);
        return -1;
    }

    // Send the data
    if (send(client_socket, data, size, 0) < 0)
    {
        fprintf(stderr, "Error: send() failed\n");
        close(client_socket);
        return -1;
    }

    // Close the connection
    close(client_socket);

    return 0;
}