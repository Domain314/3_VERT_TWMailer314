
#include "ServerLogic.hpp"

ServerLogic::ServerLogic(int port, int* _abortRequested, int* greeting_socket, int* communication_socket, FileOrganizer* fileOrganizer) {
    this->fileOrganizer = fileOrganizer;
    this->g_abortRequested = _abortRequested;
//    this->greeting_socket = greeting_socket;
//    this->communication_socket = communication_socket;
    g_port = port;
    conHead = createNode(g_port);
    conHead->com_socket = new ComSocket(g_port, fileOrganizer, g_abortRequested);

    while (!*g_abortRequested) {

        printf("prepare com socket\n");
        ++g_port;
        ConNode* node = createNode(g_port);
        node->com_socket = new ComSocket(g_port, fileOrganizer, g_abortRequested);
        getLastNode(conHead)->next = node;
        conHead->com_socket->runRouting(node->com_socket);
        node->com_socket->StartInternalThread();
    }

    ConNode* temp = conHead->next;
    while (temp != nullptr) {
        temp->com_socket->WaitForInternalThreadToExit();
        temp->com_socket->closeConnection();
        printf("port %d done.\n", temp->com_socket->port);
        temp = temp->next;
    }
    conHead->com_socket->closeConnection();
}

ServerLogic::~ServerLogic() {
    ConNode* temp = conHead, *next = conHead->next;
    while (next != nullptr) {
        delete(temp->com_socket);
        delete(temp);
        temp = temp->next;
        next = next->next;
    }
    delete(temp->com_socket);
    delete(temp);
    delete(fileOrganizer);
}

//bool ServerLogic::createConnection(int *_socket, int port, sockaddr_in* address) {
//    return true;
//}

//bool ServerLogic::createConnection(ConNode* node) {
//    int reuseValue = 1;
//
//    // Socket creation
//    if ((node->greet_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//        perror("Socket error"); // errno set by socket()
//        return false;
//    }
//
//    // Set socket options
//    if (setsockopt(node->greet_socket,
//                   SOL_SOCKET,
//                   SO_REUSEADDR,
//                   &reuseValue,
//                   sizeof(reuseValue)) == -1) {
//        perror("set socket options - reuseAddr");
//        return false;
//    }
//
//    if (setsockopt(node->greet_socket,
//                   SOL_SOCKET,
//                   SO_REUSEPORT,
//                   &reuseValue,
//                   sizeof(reuseValue)) == -1) {
//        perror("set socket options - reusePort");
//        return false;
//    }
//
//    // Init Address
////    memset(node->address, 0, sizeof(*node->address));
//    node->address->sin_family = AF_INET;
//    node->address->sin_addr.s_addr = INADDR_ANY;
//    node->address->sin_port = htons(g_port);
//
//    // Assign address with port to socket
//    if (bind(node->greet_socket, (struct sockaddr *)node->address, sizeof(*node->address)) == -1) {
//        perror("bind error");
//        return false;
//    }
//
//    // Allow connection establishing
//    if (listen(node->greet_socket, 5) == -1) {
//        perror("listen error");
//        return false;
//    }
//
//    return true;
//}

//void ServerLogic::setRouting(ConNode* node) {
//
//}

//void ServerLogic::runRouting(ConNode *node) {
//    socklen_t addrlen;
//    // waiting loop for connections
//    while (!*g_abortRequested) {
//        printf("Waiting for connections...\n");
//
//        // accept connection
//        addrlen = sizeof(struct sockaddr_in);
//        if ((node->com_socket = accept(node->greet_socket,
//                                       (struct sockaddr *)&node->cliAdress,
//                                       &addrlen)) == -1) {
//            if (*g_abortRequested) {
//                perror("accept error after aborted");
//            }
//            else {
//                perror("accept error");
//            }
//            break;
//        }
//
//        // start communication
////        printf("Client connected from %s:%d...\n",
////               inet_ntoa(node->cliAdress->sin_addr),
////               ntohs(node->cliAdress->sin_port));
//
//        ConNode* comNode = initSocket(g_port, false);
//        getLastNode(node)->next = comNode;
//
//        char buffer[BUF];
//        strcpy(buffer, to_string(comNode->port).c_str());
//        if (send(node->com_socket, buffer, strlen(buffer), 0) == -1) {
//            perror("send failed");
//            return;
//        }
//        setRouting(comNode);
////        if (isRouter) {
////
////        } else {
////            clientCommunication(&node->com_socket);
////        }
////        InternalThreadEntry(&conNode->com_socket);
////        WaitForInternalThreadToExit();
////        pthread_t th;
////        ths.push_back(th);
////        pthread_create(&th, NULL, &ServerLogic::clientCommunication, this);
////        ths.push_back(&th1);
////        pthread_join(th, NULL);
////        for (int i = 0; i < ths.size(); ++i) {
////            ths.at(i)->join();
////        }
////        thread th1(&ServerLogic::clientCommunication, ref(conNode->com_socket));
//        // returnValue can be ignored
//        node->com_socket = -1;
//        node->greet_socket = -1;
//    }
//}

//ConNode* ServerLogic::initSocket(int port, bool isRouter) {
//    struct sockaddr_in address, cliaddress;
//    ConNode* node = createNode(port, &address, &cliaddress);
//    if (!createConnection(node)) {
//        ++g_port;
//        return initSocket(++port, isRouter);
//    }
//    return node;
//}

//void ServerLogic::test() {
//    printf("do something");
//}
//void* ServerLogic::clientCommunicationHelper() {
//
//}
//void ServerLogic::clientCommunication(int _socket) {
//    int current_socket = _socket;
//    printf("start communication");
//    char buffer[BUF];
//    int size;
////    int *current_socket = _socket;
//
//    // Send Welcome
//    strcpy(buffer, "Welcome to Server314!\r\nPlease enter your command.\r\n");
//    if (send(current_socket, buffer, strlen(buffer), 0) == -1) {
//        perror("send failed");
//        return;
//    }
//
//    do {
//        clearBuffer(buffer);
//        // Receive
//        size = (int)recv(current_socket, buffer, BUF - 1, 0);
//        if (size == -1) {
//            if (*g_abortRequested) {
//                perror("recv error after aborted");
//            } else {
//                perror("recv error");
//            }
//            break;
//        }
//
//        if (size == 0) {
//            printf("Client closed remote socket\n"); // ignore error
//            break;
//        }
//
//        // remove ugly debug message, because of the sent newline of client
//        if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n') {
//            size -= 2;
//        }
//        else if (buffer[size - 1] == '\n') {
//            --size;
//        }
//
//        buffer[size] = '\0';
//        printf("Message received: %s\n", buffer); // ignore error
//
//
//
//        fileOrganizer->addCommandToQueue(new string(buffer), answer, &g_dir);
//
//
//
//        int counter = 10;
//        while (answer->empty()) {
//            sleep(1);
//            counter--;
//            if (counter < 0) {
//                *answer = "TIMEOUT\n";
//                break;
//            }
//        }
//        sendAnswer(&current_socket, answer);
//        printf("answer: %s\n", answer->c_str());
//
//
//
//    } while (strcmp(buffer, "quit") != 0 && !*g_abortRequested);
//
//    // closes/frees the descriptor if not already
//    if (current_socket != -1) {
//        if (shutdown(current_socket, SHUT_RDWR) == -1) {
//            perror("shutdown new_socket");
//        }
//        if (close(current_socket) == -1) {
//            perror("close new_socket");
//        }
//        current_socket = -1;
//    }
//    pthread_exit(nullptr);
//}

//void ServerLogic::sendAnswer(const int* current_socket, string* fsAnswer) {
//    printf("%s\n", fsAnswer->c_str());
//    if (send(*current_socket, fsAnswer->c_str(), fsAnswer->size(), 0) == -1) {
//        perror("send answer failed");
//        return;
//    }
//    *fsAnswer = "";
//}

//void ServerLogic::clearBuffer(char* buf) {
//    for (int i = 1; i < BUF; ++i) {
//        buf[i] = '\0';
//    }
//}

//void ServerLogic::InternalThreadEntry(int _socket) {
//    clientCommunication(_socket);
//}

