#ifndef _NETSTUFF_SEND_H
#define _NETSTUFF_SEND_H

/**
 * Send data to a server and immediately close the connection.
 * 
 * @param host The server's hostname or IP address.
 * @param port The server's port number.
 * @param data The data to send.
 * @param size The size of the data to send.
 * 
 * @return 0 on success, -1 on error.
 */
int ns_send(const char *host, int port, const void *data, unsigned int size);

#endif // _NETSTUFF_SEND_H