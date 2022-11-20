#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <ldap.h>

#include "ServerLogic.hpp"
#include "FileOrganizer.hpp"

int g_abortRequested = 0;
int greeting_socket = -1; // make vector<int> ?
int communication_socket = -1; // make vector<int> ?

void signalHandler(int sig);

int main(int argc, char **argv) {
    // Signal handling
    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        perror("signal can not be registered");
        return EXIT_FAILURE;
    }
    int port = 6543;
    string dir = "./database/";
    if (argc == 3) {
        port = stoi(argv[1]);
        dir = argv[2];
    }

    FileOrganizer* organizer = new FileOrganizer(&dir);

    ServerLogic* server = new ServerLogic(port, &g_abortRequested, &greeting_socket, &communication_socket, organizer);
    delete(server);
    delete(organizer);
    return 0;
}


void signalHandler(int sig) {
    if (sig == SIGINT) {
        printf("abort Requested... ");
        g_abortRequested = 1;

        // shutdown
        if (communication_socket != -1) {
            if (shutdown(communication_socket, SHUT_RDWR) == -1) {
                perror("shutdown com_socket");
            }
            if (close(communication_socket) == -1) {
                perror("close com_socket");
            }
            communication_socket = -1;
        }

        if (greeting_socket != -1) {
            if (shutdown(greeting_socket, SHUT_RDWR) == -1) {
                perror("shutdown greeting_socket");
            }
            if (close(greeting_socket) == -1) {
                perror("close greeting_socket");
            }
            greeting_socket = -1;
        }
    } else {
        exit(sig);
    }
}