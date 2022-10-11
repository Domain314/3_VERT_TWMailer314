#include "ServerLogic.h"

void clientCommunication(void *data, const int* abortRequested) {
    char buffer[BUF];
    int size;
    int *current_socket = (int *)data;

    // Send Welcome
    strcpy(buffer, "Welcome to Server314!\r\nPlease enter your commands...\r\n");
    if (send(*current_socket, buffer, strlen(buffer), 0) == -1) {
        perror("send failed");
        return;
    }

    do {
        // Receive
        size = recv(*current_socket, buffer, BUF - 1, 0);
        if (size == -1) {
            if (*abortRequested) {
                perror("recv error after aborted");
            } else {
                perror("recv error");
            }
            break;
        }

        if (size == 0) {
            printf("Client closed remote socket\n"); // ignore error
            break;
        }

        // remove ugly debug message, because of the sent newline of client
        if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n') {
            size -= 2;
        }
        else if (buffer[size - 1] == '\n') {
            --size;
        }

        buffer[size] = '\0';
        printf("Message received: %s\n", buffer); // ignore error

        if (send(*current_socket, "OK", 3, 0) == -1) {
            perror("send answer failed");
            return;
        }
    } while (strcmp(buffer, "quit") != 0 && !*abortRequested);

    // closes/frees the descriptor if not already
    if (*current_socket != -1) {
        if (shutdown(*current_socket, SHUT_RDWR) == -1) {
            perror("shutdown new_socket");
        }
        if (close(*current_socket) == -1) {
            perror("close new_socket");
        }
        *current_socket = -1;
    }

    return;
}