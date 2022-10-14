#include "ClientLogic.hpp"
#include "globals.hpp"

ClientLogic::ClientLogic(int argc, char **argv) {
    iniClient(argc, argv);
}

void ClientLogic::iniClient(int argc, char **argv) {
    int create_socket;
    char recvBuffer[BUF];
    string* inputBuffer;
//    string* sendMessage;
    struct sockaddr_in address;

    // Create socket
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket error");
        return;
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
        return;
    }

    // ignore return value of printf
    printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));

    int length = recv(create_socket, recvBuffer, BUF - 1, 0);
    // Receive Data
    if (!receiveErrorHandling(length)) { return; }
    else {
        recvBuffer[length] = '\0';
        printf("%s", recvBuffer);
    }

    // enable 'tab-completion'
    rl_bind_key('\t', rl_complete);

    while(1) {
        inputBuffer = new string(readline("> "));
        if (inputBuffer->size() != 0) {

            // digestInput will return true, when sendMessage is done.
            if (digestInput(inputBuffer)) {
                if ((send(create_socket, inputBuffer->c_str(), inputBuffer->size(), 0)) == -1) {
                    perror("send error");
                    break;
                }

                delete(inputBuffer);

                // Receive Feedback
                if (!receiveErrorHandling(recv(create_socket, recvBuffer, BUF - 1, 0))) { break; }
                else {
                    recvBuffer[strlen(recvBuffer)] = '\0';
                    printf("<< %s\n", recvBuffer); // ignore error
                }
            }
        }
    }


    // Close descriptor
    closeDescriptor(&create_socket);
}


bool ClientLogic::digestInput(string *input) {
    switch (input->at(0)) {
        case 'S':
            if (strcmp(input->c_str(), "SEND") == 0) {
                if (!sendLogic(input)) {

                }
                break;
            }
        case 'L':
            if (strcmp(input->c_str(), "LIST") == 0) {
                printf("LISTT\n");
                break;
            }
        case 'R':
            if (strcmp(input->c_str(), "READ") == 0) {
                printf("READD\n");
                break;
            }
        case 'D':
            if (strcmp(input->c_str(), "DEL") == 0) {
                printf("DELL\n");
                break;
            }
        case 'Q':
        case 'q':
            if (strcmp(input->c_str(), "QUIT") == 0 || strcmp(input->c_str(), "quit") == 0) {
                printf("QUITT\n");
                break;
            }
        default:
            printf("Command not Found.\n");
            printHelp();
            return false;
    }
    return true;
}

bool ClientLogic::sendLogic(string *input) {
    *input += "§";
    for (int i = 0; i < 4; ++i) {
        *input += readline(SEND_PROMPTS.at(i).c_str());
        printf("%s\n", input->c_str());
    }
    return true;
}








int ClientLogic::receiveErrorHandling(int size) {
    if (size == -1) {
        perror("recv error");
        return 0;
    } else if (size == 0) {
        printf("Server closed remote socket\n"); // ignore error
        return 0;
    }
    return size;
}

void ClientLogic::closeDescriptor(int* create_socket) {
    if (*create_socket != -1) {
        if (shutdown(*create_socket, SHUT_RDWR) == -1) {
            // invalid in case the server is gone already
            perror("shutdown create_socket");
        }
        if (close(*create_socket) == -1) {
            perror("close create_socket");
        }
        *create_socket = -1;
    }
}



void ClientLogic::printHelp() {
    printf("---------------------------------------------------------------------------\n");
    printf("Usage:\nChoose a MODE (SEND, LIST, READ, DEL, QUIT) and confirm with enter.\n\n");
    printf("SEND: Sender, Receiver, Subject, Message, DOT.\nConfirm each with Enter.\nDot+enter ends Message input.\n\n");
    printf("LIST: Username.\nLists all Messages.\n\n");
    printf("READ: Username, Message-Number\nDisplay Message.\n\n");
    printf("DEL: Username, Message-Number\nDelete Message.\n\n");
    printf("QUIT:\nClose Connection and end Client.\n");
    printf("---------------------------------------------------------------------------\n");
}





