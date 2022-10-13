#ifndef TWMAILER314_BASIC_CLIENTLOGIC_H
#define TWMAILER314_BASIC_CLIENTLOGIC_H

#include <sys/socket.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUF 1024
#define PORT 6543

enum COMMAND_MODE { SEND, LIST, READ, DEL, QUIT, NONE };


int initClient(int argc, char **argv);
int digestInput(char* buffer, char* sendMessage);
int receiveErrorHandling(int size);
void inputBufferAdjustment(char* buffer, int size);
void closeDescriptor(int* create_socket);

int sendLogic();




void printHelp();







#endif //TWMAILER314_BASIC_CLIENTLOGIC_H
