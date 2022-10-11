#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF 1024
#define PORT 6543

int initClient(int argc, char **argv);

int main(int argc, char **argv) {
    return initClient(argc, argv);
}

int initClient(int argc, char **argv) {
    int create_socket;
    char buffer[BUF];
    struct sockaddr_in address;
    int size;
    int isQuit;

    // Create socket
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket error");
        return EXIT_FAILURE;
    }

    // init address
    memset(&address, 0, sizeof(address)); // init storage with 0
    address.sin_family = AF_INET;         // IPv4
    address.sin_port = htons(PORT);
    if (argc < 2) {
        inet_aton("127.0.0.1", &address.sin_addr);
    } else {
        inet_aton(argv[1], &address.sin_addr);
    }

    // Create Connection
    if (connect(create_socket,
                (struct sockaddr *)&address,
                sizeof(address)) == -1) {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    // ignore return value of printf
    printf("Connection with server (%s) established\n",
           inet_ntoa(address.sin_addr));

    // Receive Data
    size = recv(create_socket, buffer, BUF - 1, 0);
    if (size == -1) {
        perror("recv error");
    } else if (size == 0) {
        printf("Server closed remote socket\n"); // ignore error
    } else {
        buffer[size] = '\0';
        printf("%s", buffer); // ignore error
    }

    do {
        printf(">> ");
        if (fgets(buffer, BUF, stdin) != NULL) {
            int size = strlen(buffer);
            // remove new-line signs from string at the end
            if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n') {
                size -= 2;
                buffer[size] = 0;
            } else if (buffer[size - 1] == '\n') {
                --size;
                buffer[size] = 0;
            }
            isQuit = strcmp(buffer, "quit") == 0;

            // Send Data
            if ((send(create_socket, buffer, size, 0)) == -1) {
                perror("send error");
                break;
            }

            // Receive Feedback
            size = recv(create_socket, buffer, BUF - 1, 0);
            if (size == -1) {
                perror("recv error");
                break;
            } else if (size == 0) {
                printf("Server closed remote socket\n"); // ignore error
                break;
            }
            else {
                buffer[size] = '\0';
                printf("<< %s\n", buffer); // ignore error
                if (strcmp("OK", buffer) != 0) {
                    fprintf(stderr, "<< Server error occured, abort\n");
                    break;
                }
            }
        }
    } while (!isQuit);

    // Close descriptor
    if (create_socket != -1) {
        if (shutdown(create_socket, SHUT_RDWR) == -1) {
            // invalid in case the server is gone already
            perror("shutdown create_socket");
        }
        if (close(create_socket) == -1) {
            perror("close create_socket");
        }
        create_socket = -1;
    }

    return EXIT_SUCCESS;
}
