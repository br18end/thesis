#include <stdio.h> /* Standard input/output definitions */
#include <stdlib.h> /* Standard General Utilities Library */
#include <fcntl.h> /* File Control Definitions */
#include <unistd.h> /* UNIX Standard Definitions */
#include <errno.h> /* ERROR Number Definitions */
#include <string.h> /* String function definitions */
#include <arpa/inet.h> /* Definitions for internet operations */
#include <sys/socket.h> /* Internet Protocol family */

#define PORT 5353 // Port for connections
#define BACKLOG 256 // Number of connections


int create_server();
void listen_port(int server);
int accept_client(int server);
void read_port(int client);


void main() {
    // Create server
    int server = create_server();
    // Listen port
    listen_port(server);
    // Accept connections
    int client = accept_client(server);
    // Read port
    while(1)
        read_port(client);
    // Close connection
    close(client);
}


int create_server() {
    // Server address
    struct sockaddr_in serverAddress;
    // Address Family IPv4
    serverAddress.sin_family = AF_INET;
    // To network byte order
    serverAddress.sin_port = htons(PORT);
    // Get local address
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    // socket() create the socket
    int server = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET Address Family IPv4
    // SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams.  An out-of-band data transmission mechanism may be supported.

    // Avoid the operative system block the address
    int activate = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &activate, sizeof(activate));

    // Check if errors creating the socket
    if (server == -1)
        printf("Creating socket: %s(Code:%d)\n", strerror(errno), errno);
    else 
        printf("Creating socket: %s(Code:%d)\n", strerror(errno), errno);

    // Bind socket with the address
    if (bind(server, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr)) == -1)
        printf("Binding socket: %s(Code:%d)\n", strerror(errno), errno);
    else 
        printf("Binding socket: %s(Code:%d)\n", strerror(errno), errno);

    return server;
}


void listen_port(int server) {
    // Enable port for connections
    if (listen(server, BACKLOG) == -1)
        printf("Listening port: %s(Code:%d)\n", strerror(errno), errno);
    else 
        printf("Listening port: %s(Code:%d)\n", strerror(errno), errno);
}


int accept_client(int server) {
    // Client address
    struct sockaddr_in clientAddress;
    unsigned int sizeAddr;
    // Wait for new connection
    int client = accept(server, (struct sockaddr *)&clientAddress, &sizeAddr);

    // Check if errors creating the socket
    if (client == -1)
        printf("Accepting client %d: %s(Code:%d)\n", client, strerror(errno), errno);
    else 
        printf("Accepting client %d: %s(Code:%d)\n", client, strerror(errno), errno);

    return client;
}


void read_port(int client) {  
    // Create buffer to store the data received
    char read_buffer[256];

    // Read the data
    int bytes_read = recv(client, read_buffer, sizeof(read_buffer), 0);

    // Check if errors reading port
    if (bytes_read == -1)
        printf("Reading port: %s(Code:%d)\n", strerror(errno), errno);
    else {
        // Print the number of bytes read
        printf("Reading %d bytes: %s(Code:%d)\n", bytes_read, strerror(errno), errno);
    }
}