#ifndef TWMAILER314_BASIC_FILEORGANIZER_HPP
#define TWMAILER314_BASIC_FILEORGANIZER_HPP

#include <sys/stat.h>
#include <sys/wait.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <random>

#include <atomic>
#include <mutex>

namespace fs = std::filesystem;

using namespace std;

// Normal distribution. a = average, f = fluctuation
static int randomInt(int a, int f)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::normal_distribution<> dis(a, f);
    return std::abs((int)dis(gen));
}

struct Node {
    string* data;
    Node* next = nullptr;
};

class FileOrganizer {
public:
    FileOrganizer();
    bool addCommandToQueue(string* command);

private:
    atomic_uint queueCounter = 0;
    vector<string>* queueCommands = new vector<string>();
    atomic_bool isSearching = false;
    Node* headQueueCommands = nullptr;
    mutex pushMutex;
    mutex popMutex;

    void initSearch();
    void digestCommand(string* command);

    void sendLogic(string* command);
    void listLogic();
    void readLogic();
    void delLogic();
    void quitLogic();

    void initUserDir(string* name);
    void createUserDir(string* dir);
    void initNewMail(string* sender, string* receiver, string* subject, string* message);
    void saveMail(string* dir, Node* mail);
    Node* constructNewMail(string* sender, string* receiver, string* subject, string* message);


    string* extractNextSubstring(int* thisDivider, int* nextDivider, string* command);
    string* extractRemainingSubstring(int* thisDivider, int* nextDivider, string* command);

    void pushStack(string* command);
    void pushList(string* str, Node* head);
    Node* getLast(Node* head);
    bool isStackEmpty();
    string* popStack();


};


#endif //TWMAILER314_BASIC_FILEORGANIZER_HPP
