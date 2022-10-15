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
    bool isQuit = false;
    void iniClient(int argc, char **argv);
    static int receiveErrorHandling(int size);

    bool digestInput(string* input);
    bool sendLogic(string* input);
    bool listLogic(string* input);
    bool readLogic(string* input);
    bool delLogic(string* input);


    void closeDescriptor(int* create_socket);
    void printHelp();
    void printError(string error);

    void readInput(char* prompt, string* destination);
    bool readInput(char* prompt, string* destination, int maxSize);
};


#endif //TWMAILER314_BASIC_CLIENTLOGIC_HPP
