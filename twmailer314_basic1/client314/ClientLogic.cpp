#include "ClientLogic.hpp"
#include "globals.hpp"

ClientLogic::ClientLogic(int argc, char **argv) {
    iniClient(argc, argv);
}

void ClientLogic::iniClient(int argc, char **argv) {
    if (argc == 3) {
        g_ip = argv[1];
        g_port = stoi(argv[2]);
    }
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
    address.sin_port = htons(g_port);
    if (argc < 2) {
        inet_aton(g_ip.c_str(), &address.sin_addr);
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

    int length = (int)recv(create_socket, recvBuffer, BUF - 1, 0);
    // Receive Data
    if (!receiveErrorHandling(length)) { return; }
    else {
        recvBuffer[length] = '\0';
        printf("%s", recvBuffer);
    }
    clearBuffer(recvBuffer);

    // enable 'tab-completion'
    rl_bind_key('\t', rl_complete);

    while(true) {
        inputBuffer = new string();
        readInput((char*)PROMPT_MES.at("empty").c_str(), inputBuffer);
        if (!inputBuffer->empty()) {

            // digestInput will return true, when sendMessage is done.
            if (digestInput(inputBuffer)) {
                if ((send(create_socket, inputBuffer->c_str(), inputBuffer->size(), 0)) == -1) {
                    perror("send error");
                    break;
                }

                // Receive Feedback
                if (!receiveErrorHandling((int)recv(create_socket, recvBuffer, BUF - 1, 0))) { break; }
                else {
                    recvBuffer[strlen(recvBuffer)] = '\0';
                    printf("<< %s\n", recvBuffer); // ignore error
                    clearBuffer(recvBuffer);
                }
            } else {
                if (isQuit) { break; }
            }
        }
        delete(inputBuffer);
    }


    // Close descriptor
    closeDescriptor(&create_socket);
}


bool ClientLogic::digestInput(string *input) {
    switch (input->at(0)) {
        case 'S':
            if (strcmp(input->c_str(), "SEND§") == 0) {
                if (!sendLogic(input)) {

                }
                break;
            }
        case 'L':
            if (strcmp(input->c_str(), "LIST§") == 0) {
                if (!listLogic(input)) {

                }
                break;
            }
        case 'R':
            if (strcmp(input->c_str(), "READ§") == 0) {
                if (!readLogic(input)) {

                }
                break;
            }
        case 'D':
            if (strcmp(input->c_str(), "DEL§") == 0) {
                if (!delLogic(input)) {

                }
                break;
            }
        case 'Q':
        case 'q':
            if (strcmp(input->c_str(), "QUIT§") == 0 || strcmp(input->c_str(), "quit§") == 0) {
                printf("Quit now.\n");
                isQuit = true;
                return false;
            }
        default:
            printf("Command not Found.\n");
            printHelp();
            return false;
    }
    return true;
}

bool ClientLogic::sendLogic(string *sendMessage) {
    for (int i = 0; i < 4; ++i) {
        readInput((char *) SEND_PROMPTS.at(i).c_str(), sendMessage);
    }
    while (true) {
        if (!readInput((char*)PROMPT_MES.at("empty").c_str(), sendMessage, BUF-sendMessage->size()-8)) {
            break;
        }
    }
    sendMessage->append("%");
    return true;
}

bool ClientLogic::listLogic(string *input) {
    readInput((char*)PROMPT_MES.at("username").c_str(), input);
    input->append("%");
    return true;
}

bool ClientLogic::readLogic(string *input) {
    readInput((char*)PROMPT_MES.at("username").c_str(), input);
    readInput((char*)PROMPT_MES.at("mes-num").c_str(), input);
    input->append("%");
    return true;
}

bool ClientLogic::delLogic(string *input) {
    readInput((char*)PROMPT_MES.at("username").c_str(), input);
    readInput((char*)PROMPT_MES.at("mes-num").c_str(), input);
    input->append("%");
    return true;
}




void ClientLogic::readInput(char* prompt, string *destination) {
    string* input = new string();
    *input = readline(prompt);
    destination->append(*input);
    destination->append("§");
    delete(input);
}

bool ClientLogic::readInput(char* prompt, string *destination, int maxSize) {
    printf("%d\n", maxSize);
    if (maxSize < 1) {
        printf("No Space left. Sending now.\n");
        return false;
    }

    string* input = new string();
    *input = readline(prompt);
    if (*input == ".") { delete(input); return false; }
    if (input->size() > maxSize) {
        printError((string*)"line-too-long");
        printf("%d characters left.\n", maxSize);
    } else {
        destination->append(*input);
        destination->append("§");
    }
    delete(input);
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

void ClientLogic::printError(string* error) {
    printf("%s", ERROR_MES.at(*error).c_str());
}

void ClientLogic::clearBuffer(char* buf) {
    for (int i = 1; i < BUF; ++i) {
        buf[i] = '\0';
    }
}








