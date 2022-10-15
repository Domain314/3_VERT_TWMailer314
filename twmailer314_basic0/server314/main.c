#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "ServerLogic.h"

#define PORT 6543

int g_abortRequested = 0;
int greeting_socket = -1;
int communication_socket = -1;

void signalHandler(int sig);
int initSocket();

int main(void) {
    return initSocket();
}

int initSocket() {
    socklen_t addrlen;
    struct sockaddr_in address, cliaddress;
    int reuseValue = 1;

    // Signal handling
    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        perror("signal can not be registered");
        return EXIT_FAILURE;
    }

    // Socket creation
    if ((greeting_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket error"); // errno set by socket()
        return EXIT_FAILURE;
    }

    // Set socket options
    if (setsockopt(greeting_socket,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuseValue,
                   sizeof(reuseValue)) == -1) {
        perror("set socket options - reuseAddr");
        return EXIT_FAILURE;
    }

    if (setsockopt(greeting_socket,
                   SOL_SOCKET,
                   SO_REUSEPORT,
                   &reuseValue,
                   sizeof(reuseValue)) == -1) {
        perror("set socket options - reusePort");
        return EXIT_FAILURE;
    }

    // Init Address
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Assign address with port to socket
    if (bind(greeting_socket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind error");
        return EXIT_FAILURE;
    }

    // Allow connection establishing
    if (listen(greeting_socket, 5) == -1) {
        perror("listen error");
        return EXIT_FAILURE;
    }

    // waiting loop for connections
    while (!g_abortRequested) {
        printf("Waiting for connections...\n");

        // accept connection
        addrlen = sizeof(struct sockaddr_in);
        if ((communication_socket = accept(greeting_socket,
                                           (struct sockaddr *)&cliaddress,
                                           &addrlen)) == -1) {
            if (g_abortRequested) {
                perror("accept error after aborted");
            }
            else {
                perror("accept error");
            }
            break;
        }

        // start communication
        printf("Client connected from %s:%d...\n",
               inet_ntoa(cliaddress.sin_addr),
               ntohs(cliaddress.sin_port));
        clientCommunication(&communication_socket, &g_abortRequested); // returnValue can be ignored
        communication_socket = -1;
    }

    return EXIT_SUCCESS;
}

void signalHandler(int sig) {
    if (sig == SIGINT) {
        printf("abort Requested... "); // ignore error
        g_abortRequested = 1;

        // shutdown
        if (communication_socket != -1) {
            if (shutdown(communication_socket, SHUT_RDWR) == -1) {
                perror("shutdown com_socket");
            }
            if (close(communication_socket) == -1) {
                perror("close com_socket");
            }
            communication_socket = -1;
        }

        if (greeting_socket != -1) {
            if (shutdown(greeting_socket, SHUT_RDWR) == -1) {
                perror("shutdown greeting_socket");
            }
            if (close(greeting_socket) == -1) {
                perror("close greeting_socket");
            }
            greeting_socket = -1;
        }
    } else {
        exit(sig);
    }
}