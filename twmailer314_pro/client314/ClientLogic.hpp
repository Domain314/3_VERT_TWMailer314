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

#define BUF 1024

using namespace std;

class ClientLogic {
public:
    ClientLogic(int argc, char** argv);

private:
    bool isQuit = false;
    string g_ip = "127.0.0.1";
    int g_port = 6543;
    string* userName = new string();
    int isLoggedIn = 0;

    void initClient(int argc, char **argv);
    static int receiveErrorHandling(int size);

    bool digestInput(string* input);

    void loginLogic(string* input);
    void logoutLogic(string* input);
    static void sendLogic(string* input, string* userName);
    static void listLogic(string* input, string* userName);
    static void readLogic(string* input, string* userName);
    static void delLogic(string* input, string* userName);

    static void closeDescriptor(int* create_socket);
    static void printHelp();
    static void printError(string error);

    static void addArg(string* destination, string* arg);
    void readInput(string* destination, bool isLogin);
    static void readInput(char* prompt, string* destination);
    static bool readInput(char* prompt, string* destination, int maxSize);

    static void clearBuffer(char* buf);
    void clearUserName();
};


#endif //TWMAILER314_BASIC_CLIENTLOGIC_HPP
