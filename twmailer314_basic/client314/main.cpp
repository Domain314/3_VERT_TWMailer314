#include "ClientLogic.hpp"

int main(int argc, char **argv) {
    ClientLogic* client = new ClientLogic(argc, argv);
    delete(client);
    return 0;
}

