#include "ClientLgic.hpp"

char* sendPrompts[4] = {"Sender: ", "Receiver: ", "Subject: ", "Message: "};

int initClient(int argc, char **argv) {
    int create_socket;
    char recvBuffer[BUF];
    char* inputBuffer;
    char* sendMessage;

//    char shell_prompt[100];
    struct sockaddr_in address;

//    int state = 0;

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
    printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));

    int length = recv(create_socket, recvBuffer, BUF - 1, 0);
    // Receive Data
    if (!receiveErrorHandling(length)) { return EXIT_FAILURE; }
    else {
        recvBuffer[length] = '\0';
        printf("%s", recvBuffer);
    }

    // enable 'tab-completion'
    rl_bind_key('\t', rl_complete);

    while(1) {
//        char* cwd = getcwd(NULL, 1024); // memory leak prevention
//        snprintf(shell_prompt, sizeof(shell_prompt), "%s:%s $ ", getenv("USER"), cwd);
//        free(cwd); // memory leak prevention

        inputBuffer = readline("> ");
        if (inputBuffer != NULL) {
            int size = strlen(inputBuffer);
            inputBufferAdjustment(inputBuffer, size);

            if (strcmp(inputBuffer, "quit") == 0) break;

            // digestInput will return true, when sendMessage is done.
            if (digestInput(inputBuffer, sendMessage)) {
                if ((send(create_socket, inputBuffer, size, 0)) == -1) {
                    perror("send error");
                    break;
                }
                sendMessage = (char*) malloc(100);
                free(sendMessage);
                sendMessage = NULL;
                g_mode = NONE;

                free(inputBuffer);

                // Receive Feedback
                if (!receiveErrorHandling(recv(create_socket, recvBuffer, BUF - 1, 0))) { break; }
                else {
                    recvBuffer[size] = '\0';
                    printf("<< %s\n", recvBuffer); // ignore error
                }
            }

        }

    }

    if (sendMessage != NULL) free(sendMessage);

    // Close descriptor
    closeDescriptor(&create_socket);

    return EXIT_SUCCESS;
}

int receiveErrorHandling(int size) {
    if (size == -1) {
        perror("recv error");
        return 0;
    } else if (size == 0) {
        printf("Server closed remote socket\n"); // ignore error
        return 0;
    }
    return size;
}

// remove new-line signs from string at the end
void inputBufferAdjustment(char* buffer, int size) {
    if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n') {
        size -= 2;
        buffer[size] = 0;
    } else if (buffer[size - 1] == '\n') {
        --size;
        buffer[size] = 0;
    }
}

int digestInput(char* buffer, char* sendMessage) {
    switch (buffer[0]) {
        case 'S':
            if (strcmp(buffer, "SEND") == 0) {
                if (sendLogic(buffer, sendMessage)) {

                }
                break;
            }
        case 'L':
            if (strcmp(buffer, "LIST") == 0) {
                printf("LISTT");
                g_mode = LIST;
                break;
            }
        case 'R':
            if (strcmp(buffer, "READ") == 0) {
                printf("READD");
                g_mode = READ;
                break;
            }
        case 'D':
            if (strcmp(buffer, "DEL") == 0) {
                printf("DELL");
                g_mode = DEL;
                break;
            }
        case 'Q':
        case 'q':
            if (strcmp(buffer, "QUIT") == 0 || strcmp(buffer, "quit") == 0) {
                printf("QUITT");
                g_mode = QUIT;
                break;
            }
        default:
            printf("Command not Found.\n");
            printHelp();
            return 0;
    }
    return 1;

    if (g_mode == NONE) {
        printf("NONE\n");

        return 0;
    } else {
        switch (g_mode) {
            case SEND: printf("send\n");
            case LIST: printf("list\n");
            case READ: printf("read\n");
            case DEL: printf("del\n");
            case QUIT: printf("quit\n");
            case NONE: printf("none\n");
        }
        printf("%d\n", g_mode);
    }
    buffer = sendMessage;
    sendMessage = buffer;
    return 1;
}

int sendLogic(char* buffer, char* sendMessage) {
//    int leng = strlen(buffer);
//    sendMessage = malloc(strlen(buffer)+1);
//    strcpy(sendMessage, buffer);
//    sendMessage[strlen(buffer)] = "§";
////    sendMessage[strlen(buffer)+1] = "\0";
//    for (int i = 0; i < 4; ++i) {
//        buffer = readline(sendPrompts[i]);
//        int len = strlen(sendMessage)+strlen(buffer)+1;
//        sendMessage = (char*)malloc(len);
////        strcpy(sendMessage, sendMessage);
//        strcat(sendMessage, buffer);
//        printf("buffer: %s\n", sendMessage);
//    }
    return 0;

}

void closeDescriptor(int* create_socket) {
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



void printHelp() {
    printf("---------------------------------------------------------------------------\n");
    printf("Usage:\nChoose a MODE (SEND, LIST, READ, DEL, QUIT) and confirm with enter.\n\n");
    printf("SEND: Sender, Receiver, Subject, Message, DOT.\nConfirm each with Enter.\nDot+enter ends Message input.\n\n");
    printf("LIST: Username.\nLists all Messages.\n\n");
    printf("READ: Username, Message-Number\nDisplay Message.\n\n");
    printf("DEL: Username, Message-Number\nDelete Message.\n\n");
    printf("QUIT:\nClose Connection and end Client.\n");
    printf("---------------------------------------------------------------------------\n");
}