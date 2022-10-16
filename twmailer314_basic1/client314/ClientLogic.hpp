#ifndef TWMAILER314_BASIC_CLIENTLOGIC_HPP
#define TWMAILER314_BASIC_CLIENTLOGIC_HPP

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

    static bool sendLogic(string* input);
    static bool listLogic(string* input);
    static bool readLogic(string* input);
    static bool delLogic(string* input);

    static void closeDescriptor(int* create_socket);
    static void printHelp();
    static void printError(string* error);

    static void readInput(char* prompt, string* destination);
    static bool readInput(char* prompt, string* destination, int maxSize);

    static void clearBuffer(char* buf);
};


#endif //TWMAILER314_BASIC_CLIENTLOGIC_HPP
