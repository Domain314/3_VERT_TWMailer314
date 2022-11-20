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
#include <pthread.h>


#include "FileOrganizer.hpp"
#include "NodeList.hpp"
#include "ComSocket.hpp"

//#define PORT 6543
#define BUF 1024

using namespace std;

class ServerLogic {
public:
    ServerLogic(int port, int* g_abortRequested, int* greeting_socket, int* communication_socket, FileOrganizer* fileOrganizer);
    ~ServerLogic();
private:
    FileOrganizer* fileOrganizer;
    int* g_abortRequested;

    ConNode* conHead;

    uint g_port = 6543;
    string g_dir = "./database/";

};


#endif //TWMAILER314_BASIC_SERVERLOGIC_HPP
