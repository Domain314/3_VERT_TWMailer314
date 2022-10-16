#ifndef TWMAILER314_BASIC_SERVERLOGIC_HPP
#define TWMAILER314_BASIC_SERVERLOGIC_HPP

#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>

#include "FileOrganizer.hpp"

#define PORT 6543
#define BUF 1024

using namespace std;

typedef void (*AnswerCallback)(int*);

class ServerLogic {
public:
    ServerLogic(int argc, char** argv, int* g_abortRequested, int* greeting_socket, int* communication_socket, FileOrganizer* fileOrganizer);

private:
    FileOrganizer* fileOrganizer;
    int* g_abortRequested;
    int* greeting_socket;
    int* communication_socket;
    string* answer = new string("");

    void initSocket();
    void clientCommunication(void *data, const int* abortRequested);

    static void sendAnswer(const int* current_socket, string* answer);
    static void clearBuffer(char* buf);


};


#endif //TWMAILER314_BASIC_SERVERLOGIC_HPP
