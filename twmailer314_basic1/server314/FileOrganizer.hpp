#ifndef TWMAILER314_BASIC_FILEORGANIZER_HPP
#define TWMAILER314_BASIC_FILEORGANIZER_HPP

#include <cstring>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

// for multithreading
#include <atomic>
#include <mutex>

namespace fs = std::filesystem;

using namespace std;

// Real distribution. [min - max)
static int randomInt(int min, int max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(min, max);
  return std::abs((int)dis(gen));
}

// Node struct to link mails and answers
// see pushStack() & pushList()
struct Node {
  string *data{};
  string *answer{};
  Node *next{};
};

class FileOrganizer {
public:
  FileOrganizer(string *dir);

  // QUEUE LOGIC 1
  void addCommandToQueue(string *command, string *answer, string *dir);

private:
  atomic_uint queueCounter = 0;
  atomic_bool isSearching = false;
  Node *headQueueCommands = nullptr;
  mutex pushMutex;
  mutex popMutex;
  string *mailSpoolDir{};

  // QUEUE LOGIC 2
  void initSearch();
  void digestCommand(Node *node);

  // COMMAND LOGICS
  bool sendLogic(string *command);
  bool listLogic(Node *node);
  bool readLogic(Node *node);
  bool delLogic(Node *node);
  static bool quitLogic(Node *node);

  // FILESYSTEM WRITE
  void initUserDir(string *name);
  void createUserDir(string *dir);
  void saveMail(string *dir, Node *mail);

  // LIST LOGIC
  string *constructListResult(string *name);
  static string *searchDirectory(fs::path *path, int *counter);
  static string *extractID(string *path);
  static string *extractSubject(fs::path *path);

  // READ LOGIC
  string *constructReadMail(string *name, string *mesNum);
  static string *extractMail(fs::path *path);

  // DEL LOGIC
  bool deleteMail(string *name, string *mesNum);

  // MAIL CREATION
  void initNewMail(string *sender, string *receiver, string *subject,
                   string *message);
  static Node *constructNewMail(string *sender, string *receiver,
                                string *subject, string *message);

  // HELPER FUNCTIONS
  static string *extractNextSubstring(int *thisDivider, int *nextDivider,
                                      string *command);
  static string *extractRemainingSubstring(int *thisDivider, int *nextDivider,
                                           string *command);

  // STACK & LINKED LIST FUNCTIONS
  void pushStack(string *command, string *answer);
  static void pushList(string *str, Node *head);
  static Node *getLast(Node *head);
  bool isStackEmpty();
  Node *popStack();
};

#endif // TWMAILER314_BASIC_FILEORGANIZER_HPP
