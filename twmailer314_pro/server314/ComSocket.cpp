
#include "ComSocket.hpp"

ComSocket::ComSocket(int _port, FileOrganizer* _organizer, int* _abortRequested) {
    port = _port;
    organizer = _organizer;
    abortRequested = _abortRequested;
    address = new sockaddr_in();
    cliAdress = new sockaddr_in();
    initSocket();
    printf("Server set up at port:%d\n", port);
}

ComSocket::~ComSocket() {
    delete(address);
    delete(cliAdress);
    delete(answer);
}

void ComSocket::InternalThreadEntry() {
    printf("run com..\n");
    clientCommunication();
}

void ComSocket::initSocket() {
    if (!createConnection()) {
        ++port;
        return initSocket();
    }
}

bool ComSocket::createConnection() {
    int reuseValue = 1;

    // Socket creation
    if ((com_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket error"); // errno set by socket()
        return false;
    }

    // Set socket options
    if (setsockopt(com_socket,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuseValue,
                   sizeof(reuseValue)) == -1) {
        perror("set socket options - reuseAddr");
        return false;
    }

    if (setsockopt(com_socket,
                   SOL_SOCKET,
                   SO_REUSEPORT,
                   &reuseValue,
                   sizeof(reuseValue)) == -1) {
        perror("set socket options - reusePort");
        return false;
    }

    // Init Address
    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);

    // Assign address with port to socket
    if (bind(com_socket, (struct sockaddr *)address, sizeof(*address)) == -1) {
        perror("bind error");
        return false;
    }

    // Allow connection establishing
    if (listen(com_socket, 5) == -1) {
        perror("listen error");
        return false;
    }

    return true;
}

//void ComSocket::runServer() {
//    socklen_t addrlen;
//    // waiting loop for connections
//    while (!*abortRequested) {
//        printf("Waiting for connections...\n");
//
//        // accept connection
//        addrlen = sizeof(struct sockaddr_in);
//        if ((com_socket = accept(com_socket,
//                                       (struct sockaddr *)cliAdress,
//                                       &addrlen)) == -1) {
//            if (*abortRequested) {
//                perror("accept error after aborted");
//            }
//            else {
//                perror("accept error");
//            }
//            break;
//        }
//
////      start communication
//        printf("Client connected from %s:%d...\n",
//               inet_ntoa(cliAdress->sin_addr),
//               ntohs(cliAdress->sin_port));
//
//
//        clientCommunication();
//    }
//    closeConnection();
//}

void ComSocket::clientCommunication() {
    printf("start communication");
    char buffer[BUF];
    int size;

    // Send Welcome
    sendAnswer(new string("Welcome to Server314:\r\nPlease enter your command.\r\n"));

    do {
        clearBuffer(buffer);
        // Receive
        size = (int)recv(com_socket, buffer, BUF - 1, 0);
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
        printf("Message received: %s\n", buffer);

        string* input = new std::string(buffer);
        if (strcmp(input->substr(0,5).c_str(), "LOGIN") == 0) {
            in_addr ipAddr = cliAdress->sin_addr;
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN);
            input->append(ip);
        }

        organizer->addCommandToQueue(input, answer);

        int counter = 10;
        while (answer->empty()) {
            sleep(1);
            counter--;
            if (counter < 0) {
                *answer = "TIMEOUT\n";
                break;
            }
        }

        sendAnswer(answer);
        answer = new string("");

    } while (strcmp(buffer, "quit") != 0 && !*abortRequested);

    closeConnection();
}

void ComSocket::sendAnswer(std::string* fsAnswer) {
    printf("%s\n", fsAnswer->c_str());
    if (send(com_socket, fsAnswer->c_str(), fsAnswer->size(), 0) == -1) {
        perror("send answer failed");
        return;
    }
    delete fsAnswer;
}

void ComSocket::closeConnection() {
    if (com_socket != -1) {
        if (shutdown(com_socket, SHUT_RDWR) == -1) {
            printf("shutdown new_socket");
        }
        if (close(com_socket) == -1) {
            printf("close new_socket");
        }
        com_socket = -1;
    }
}

void ComSocket::clearBuffer(char* buf) {
    for (int i = 1; i < BUF; ++i) {
        buf[i] = '\0';
    }
}

void ComSocket::runRouting(ComSocket* comSocket) {
    socklen_t addrlen;
    printf("Routing waiting for connections...\n");

    // accept connection
    addrlen = sizeof(struct sockaddr_in);
    if ((comSocket->com_socket = accept(com_socket,
                             (struct sockaddr *)comSocket->cliAdress,
                             &addrlen)) == -1) {
        if (*abortRequested) {
            perror("accept error after aborted");
        }
        else {
            perror("accept error");
        }
        return;
    }

    printf("Client connected from %s:%d...\n",
           inet_ntoa(comSocket->cliAdress->sin_addr),
           ntohs(comSocket->cliAdress->sin_port));
}


