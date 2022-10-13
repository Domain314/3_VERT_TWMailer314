#ifndef TWMAILER314_BASIC_SERVERLOGIC_H
#define TWMAILER314_BASIC_SERVERLOGIC_H

#define BUF 1024

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


void clientCommunication(void *data, const int* abortRequested);



#endif //TWMAILER314_BASIC_SERVERLOGIC_H
