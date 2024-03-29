#include <csignal>
#include "ServerLogic.hpp"

ServerLogic::ServerLogic(int port, int* g_abortRequested, int* greeting_socket, int* communication_socket, FileOrganizer* fileOrganizer) {
    this->fileOrganizer = fileOrganizer;
    this->g_abortRequested = g_abortRequested;
    this->greeting_socket = greeting_socket;
    this->communication_socket = communication_socket;
    g_port = port;
    initSocket();
}

// Initialize all socket relevant fields. 
void ServerLogic::initSocket() {
    socklen_t addrlen;
    struct sockaddr_in address, cliaddress;
    int reuseValue = 1;

    // Socket creation
    if ((*greeting_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket error"); // errno set by socket()
        return;
    }

    // Set socket options
    if (setsockopt(*greeting_socket,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuseValue,
                   sizeof(reuseValue)) == -1) {
        perror("set socket options - reuseAddr");
        return;
    }

    if (setsockopt(*greeting_socket,
                   SOL_SOCKET,
                   SO_REUSEPORT,
                   &reuseValue,
                   sizeof(reuseValue)) == -1) {
        perror("set socket options - reusePort");
        return;
    }

    // Init Address
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(g_port);

    // Assign address with port to socket
    if (bind(*greeting_socket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind error");
        return;
    }

    // Allow connection establishing
    if (listen(*greeting_socket, 5) == -1) {
        perror("listen error");
        return;
    }

    // waiting loop for connections
    while (!*g_abortRequested) {
        printf("Waiting for connections...\n");

        // accept connection
        addrlen = sizeof(struct sockaddr_in);
        if ((*communication_socket = accept(*greeting_socket,
                                           (struct sockaddr *)&cliaddress,
                                           &addrlen)) == -1) {
            if (*g_abortRequested) {
                perror("accept error after aborted");
            }
            else {
                perror("accept error");
            }
            break;
        }

        // start communication
        printf("Client connected from %s:%d...\n", inet_ntoa(cliaddress.sin_addr), ntohs(cliaddress.sin_port));
        clientCommunication(communication_socket, g_abortRequested); // returnValue can be ignored
        *communication_socket = -1;
    }
}

// Communication logic.
void ServerLogic::clientCommunication(void *data, const int *abortRequested) {
    char buffer[BUF];
    int size;
    int *current_socket = (int *)data;

    // Send Welcome
    strcpy(buffer, "Welcome to Server314!\r\nPlease enter your command.\r\n");
    if (send(*current_socket, buffer, strlen(buffer), 0) == -1) { perror("send failed"); return; }

    do {
        clearBuffer(buffer);
        // Receive
        size = (int)recv(*current_socket, buffer, BUF - 1, 0);
        if (size == -1) {
            if (*abortRequested) { perror("recv error after aborted"); } 
            else { perror("recv error"); }
            break;
        }
        if (size == 0) { printf("Client closed remote socket\n"); break; }

        // remove ugly debug message, because of the sent newline of client
        if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n') { size -= 2; } 
        else if (buffer[size - 1] == '\n') { --size; }

        buffer[size] = '\0';
        printf("Message received: %s\n", buffer); // ignore error


        // put command in queue of fileOrganizer
        fileOrganizer->addCommandToQueue(new string(buffer), answer, &g_dir);

        // wait 10sek for answer and return TIMEOUT, if no answer was given.
        int counter = 10;
        while (answer->empty()) {
            sleep(1);
            counter--;
            if (counter < 0) {
                *answer = "TIMEOUT\n";
                break;
            }
        }
        sendAnswer(current_socket, answer);
        printf("answer: %s\n", answer->c_str());

    } while (strcmp(buffer, "quit") != 0 && !*abortRequested);

    // closes/frees the descriptor if not already
    if (*current_socket != -1) {
        if (shutdown(*current_socket, SHUT_RDWR) == -1) { perror("shutdown new_socket"); }
        if (close(*current_socket) == -1) { perror("close new_socket"); }
        *current_socket = -1;
    }
}

// Send answer to client
void ServerLogic::sendAnswer(const int* current_socket, string* fsAnswer) {
    printf("Answer: %s\n", fsAnswer->c_str());
    if (send(*current_socket, fsAnswer->c_str(), fsAnswer->size(), 0) == -1) {
        perror("send answer failed");
        return;
    }
    *fsAnswer = "";
}

// clear buffer (bugfix)
void ServerLogic::clearBuffer(char* buf) {
    for (int i = 1; i < BUF; ++i) { buf[i] = '\0'; }
}

