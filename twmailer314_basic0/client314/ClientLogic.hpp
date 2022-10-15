#ifndef TWMAILER314_BASIC_CLIENTLOGIC_HPP
#define TWMAILER314_BASIC_CLIENTLOGIC_HPP

//#include <readline/history.h>
#include <cstdio>
#include <sys/socket.h>
#include <readline/readline.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>

#define PORT 6543
#define BUF 1024

using namespace std;

class ClientLogic {
public:
    ClientLogic(int argc, char** argv);

private:
    void iniClient(int argc, char **argv);
    static int receiveErrorHandling(int size);

    bool digestInput(string* input);
    bool sendLogic(string* input);


    void closeDescriptor(int* create_socket);
    void printHelp();
    void printError(string* error);
};


#endif //TWMAILER314_BASIC_CLIENTLOGIC_HPP
