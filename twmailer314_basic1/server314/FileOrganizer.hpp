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

// Real distribution. [min - max)
static int randomInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return std::abs((int)dis(gen));
}

struct Node {
    string* data;
    string* answer;
    Node* next = nullptr;
};

class FileOrganizer {
public:
    FileOrganizer();
    void addCommandToQueue(string* command, string* answer);

private:
    atomic_uint queueCounter = 0;
    vector<string>* queueCommands = new vector<string>();
    atomic_bool isSearching = false;
    Node* headQueueCommands = nullptr;
    mutex pushMutex;
    mutex popMutex;

    void initSearch();
    static void digestCommand(Node* node);

    static bool sendLogic(string* command);
    static bool listLogic(Node* node);
    static bool readLogic(Node* node);
    static bool delLogic(Node* node);
    static bool quitLogic(Node* node);

    static void initUserDir(string* name);
    static void createUserDir(string* dir);
    static void initNewMail(string* sender, string* receiver, string* subject, string* message);
    static void saveMail(string* dir, Node* mail);
    static Node* constructNewMail(string* sender, string* receiver, string* subject, string* message);

    static string* extractNextSubstring(int* thisDivider, int* nextDivider, string* command);
    static string* extractRemainingSubstring(int* thisDivider, int* nextDivider, string* command);

    static string* constructListResult(string* name);
    static string* searchDirectory(fs::path* path, int* counter);
    static string* extractID(string* path);
    static string* extractSubject(fs::path* path);

    static string* constructReadMail(string* name, string* mesNum);
    static string* extractMail(fs::path* path);

    static bool deleteMail(string* name, string* mesNum);

    void pushStack(string* command, string* answer);
    static void pushList(string* str, Node* head);
    static Node* getLast(Node* head);
    bool isStackEmpty();
    Node* popStack();


};


#endif //TWMAILER314_BASIC_FILEORGANIZER_HPP
