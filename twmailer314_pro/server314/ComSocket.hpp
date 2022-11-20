#ifndef TWMAILER314_BASIC_COMSOCKET_HPP
#define TWMAILER314_BASIC_COMSOCKET_HPP

#include <cstdio>
#include <cstring>
#include <csignal>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "MThread.hpp"
#include "FileOrganizer.hpp"

#define BUF 1024

class ComSocket : public MThread {
public:
    uint port;

    ComSocket(int port, FileOrganizer* organizer, int* _abortRequested);
    ~ComSocket();

    void runRouting(ComSocket* comSocket);
    void closeConnection();

private:
    int com_socket = -1;
    sockaddr_in* address{};
    sockaddr_in* cliAdress{};

    bool isRouter;
    int* abortRequested{};
    std::string* answer = new std::string("");
    std::string dir = "./database/";
    FileOrganizer* organizer{};

    void InternalThreadEntry() override;

    void initSocket();
    bool createConnection();
    void runServer();
    void clientCommunication();
    void sendAnswer(std::string* fsAnswer);



    void clearBuffer(char* buf);
};


#endif //TWMAILER314_BASIC_COMSOCKET_HPP
