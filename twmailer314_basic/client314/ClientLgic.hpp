#ifndef TWMAILER314_BASIC_CLIENTLGIC_HPP
#define TWMAILER314_BASIC_CLIENTLGIC_HPP

#include <sys/socket.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdlib.h>

#define BUF 1024
#define PORT 6543

using namespace std;

int initClient(int argc, char **argv);
int digestInput(char* buffer, char* sendMessage);
int receiveErrorHandling(int size);
void inputBufferAdjustment(char* buffer, int size);
void closeDescriptor(int* create_socket);

int sendLogic(char* buffer, char* sendMessage);




void printHelp();





#endif //TWMAILER314_BASIC_CLIENTLGIC_HPP
