#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "ServerLogic.hpp"
#include "FileOrganizer.hpp"

#define PORT 6543

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
    FileOrganizer* organizer = new FileOrganizer();

    ServerLogic* server = new ServerLogic(argc, argv, &g_abortRequested, &greeting_socket, &communication_socket, organizer);
    delete(server);
    delete(organizer);
    return 0;
}


void signalHandler(int sig) {
    if (sig == SIGINT) {
        printf("abort Requested... "); // ignore error
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