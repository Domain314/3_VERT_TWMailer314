
#include "FileOrganizer.hpp"

// constructor sets mailSpoolDir
FileOrganizer::FileOrganizer(string* dir) {
    mailSpoolDir = new string(*dir);
}

/*****************
 *  QUEUE LOGIC  *
 *****************/
// add client command from one of n-server to queue of single filesystem, to fight race conditions
void FileOrganizer::addCommandToQueue(string *command, string* answer, string* dir) {
    try {
        pushStack(command, answer);
        queueCounter++;
        if (!isSearching) {
            initSearch();
        }
    } catch (...) {
        printf("Error adding to queue");
    }
}

// begin searching queue
void FileOrganizer::initSearch() {
    isSearching = true;
    while(headQueueCommands != nullptr) {
        try {
            digestCommand(popStack());
            queueCounter--;
        } catch (...) { printf("Unknown Error while searching.\n"); }
    }
    isSearching = false;
}

// 1. check only the first letter of command.
// 2. check remaining letters with strcmp().
// 3. run command-logic.
void FileOrganizer::digestCommand(Node *node) {
    if (node->data == nullptr) { printf("command empty\n");return; }
    printf("command: %s\n", node->data->c_str());

    switch (node->data->at(0)) {
        case 'S':
            if (strcmp(node->data->substr(0, 4).c_str(), "SEND") == 0) {
                printf("Send!\n");
                 if (sendLogic(node->data)) {
                    *node->answer = "OK!\n";
                 } else {
                    *node->answer = "ERR!\n";
                 }

            } else { printf("something went wrong"); }
            break;
        case 'L':
            if (strcmp(node->data->substr(0, 4).c_str(), "LIST") == 0) {
                printf("List!\n");
                if (!listLogic(node)) {
                    *node->answer = "0\n";
                }
            }
            break;
        case 'R':
            if (strcmp(node->data->substr(0, 4).c_str(), "READ") == 0) {
                printf("Read!\n");
                if (!readLogic(node)) {
                    *node->answer = "ERR!\n";
                }
            }
            break;
        case 'D':
            if (strcmp(node->data->substr(0, 3).c_str(), "DEL") == 0) {
                printf("Del!\n");
                if (!delLogic(node)) {
                    *node->answer = "ERR\n";
                } else {
                    *node->answer = "OK\n";
                }  
            }
            break;
        case 'Q':
        case 'q':
            if (strcmp(node->data->substr(0, 4).c_str(), "QUIT") == 0 || strcmp(node->data->substr(0, 4).c_str(), "QUIT") == 0) {
                printf("Quit now.\n");
                quitLogic(node);
                return;
            }
        default:
            printf("Command not Found.\n");
            return;
    }
}

/********************
 *  COMMAND LOGICS  *
 ********************/
// SEND: extract substrings from command to form a node-cunstruct, which represents the mail.
bool FileOrganizer::sendLogic(string* command) {
    try {
        int thisDivider = 0;
        int nextDivider = (int)command->substr(thisDivider, command->size()).find("§");
        string subString = command->substr(thisDivider, nextDivider);
        string* sender, *receiver, *subject, *message;
        sender = extractNextSubstring(&thisDivider, &nextDivider, command);
        receiver = extractNextSubstring(&thisDivider, &nextDivider, command);
        subject = extractNextSubstring(&thisDivider, &nextDivider, command);
        message = extractRemainingSubstring(&thisDivider, &nextDivider, command);
        initUserDir(sender);
        initUserDir(receiver);
        initNewMail(sender, receiver, subject, message);
        return true;
    } catch (...) { return false; }
}

// LIST: search for mails. Append answer by the search result.
bool FileOrganizer::listLogic(Node* node) {
    try {
        string* name = new string(node->data->substr(6, node->data->substr(6,node->data->size()-1).find("§")));
        node->answer->append(*constructListResult(name));
        return true;
    } catch (...) { return false; }
}

// READ: extract substrings and save node-construct (mail) as answer.
bool FileOrganizer::readLogic(Node* node) {
    try {
        int thisDivider = 0;
        int nextDivider = (int)node->data->substr(thisDivider, node->data->size()).find("§");
        string* name, *mesNum;
        name = extractNextSubstring(&thisDivider, &nextDivider, node->data);
        mesNum = extractNextSubstring(&thisDivider, &nextDivider, node->data);
        node->answer->append(*constructReadMail(name, mesNum));
        return true;
    } catch (...) { return false; }
}

// DEL: extract substrings and delete mail
bool FileOrganizer::delLogic(Node* node) {
    int thisDivider = 0;
    int nextDivider = (int)node->data->substr(thisDivider, node->data->size()).find("§");
    string* name, *mesNum;
    name = extractNextSubstring(&thisDivider, &nextDivider, node->data);
    mesNum = extractNextSubstring(&thisDivider, &nextDivider, node->data);
    return deleteMail(name, mesNum);
}

// QUIT: quits connection to client
bool FileOrganizer::quitLogic(Node* node) {
    return true;
}

/**********************
 *  FILESYSTEM WRITE  *
 **********************/
void FileOrganizer::initUserDir(string *name) {
    if (name == nullptr) return;

    createUserDir(new string(*name + "/in"));
    createUserDir(new string(*name + "/out"));
}

void FileOrganizer::createUserDir(string *dir) {
    fs::path path{(*mailSpoolDir + *dir)};
    fs::create_directories(path);
    delete(dir);
}

void FileOrganizer::saveMail(string* dir, Node* mail) {
    fs::path path{(*mailSpoolDir + *dir)};
    ofstream ofs(path);
    Node* temp = mail->next;
    while (temp != nullptr) {
        ofs << *temp->data;
        temp = temp->next;
    }
    ofs.close();
}


string *FileOrganizer::constructReadMail(string* name, string* mesNum) {
    if (name == nullptr || mesNum == nullptr) return new string("ERR");

    fs::path pathIn{*mailSpoolDir + *name + "/in/" + *mesNum};
    fs::path pathOut{*mailSpoolDir + *name + "/out/" + *mesNum};
    if (fs::exists(pathIn)) {
        return extractMail(&pathIn);
    } else if (fs::exists(pathOut)) {
        return extractMail(&pathOut);
    }
    return new string("ERR");
}

string *FileOrganizer::extractMail(fs::path* path) {
    ifstream inFile(*path, ios::in);
    if (inFile.fail()) {
        printf("Error: path does not exist!\n");
        return nullptr;
    }
    string lineStr, *result = new string();

    while (std::getline(inFile, lineStr)) {
        result->append(lineStr + "\n");
    }
    return result;
}




string *FileOrganizer::constructListResult(string *name) {
    int inCounter = 0, outCounter = 0;
    string* in = searchDirectory(new fs::path{*mailSpoolDir + *name + "/in"}, &inCounter);
    string* out = searchDirectory(new fs::path{*mailSpoolDir + *name + "/out"}, &outCounter);
    if (inCounter == 0 && outCounter == 0) {
        printf("empty\n");
        return (string*)"0";
    } else {
        name->append(":\nin (" + to_string(inCounter) + ")\n");
        name->append(*in);
        name->append("out (" + to_string(outCounter) + ")\n");
        name->append(*out);
        return name;
    }
}

string *FileOrganizer::searchDirectory(fs::path* path, int* counter) {
    if (fs::is_directory(*path)) {
        string* result = new string("");
        for (const auto & entry : fs::directory_iterator(*path)) {
            printf("entry: %s\n", entry.path().c_str());
            result->append(*extractID(new string(entry.path())));
            result->append(*extractSubject(new fs::path(entry.path())));
            *counter += 1;
        }
        return result;
    }

    return (string*)"";
}

string *FileOrganizer::extractID(string* path) {
    int i = (int)path->size()-1;
    for (; i > 0; --i) {
        if (path->at(i) == '/') break;
    }
    return new string(path->substr(i+1, path->size()-1) + ": ");
}

string *FileOrganizer::extractSubject(fs::path* path) {
    ifstream inFile(*path, ios::in);
    if (inFile.fail()) {
        printf("Error: Config not found or not able to open");
        return nullptr;
    }
    string lineStr;

    while (std::getline(inFile, lineStr)) {
        if (strcmp(lineStr.substr(0, 7).c_str(), "Subject") == 0) {
            return new string(lineStr.substr(9, lineStr.size()-1) + "\n");
        }
    }
    return nullptr;
}








void FileOrganizer::initNewMail(string* sender, string* receiver, string* subject, string* message) {
    if (sender == nullptr || receiver == nullptr || subject == nullptr) return;

    Node* mail = constructNewMail(sender, receiver, subject, message);
    string dir = *sender + "/out/" + *mail->data;
    saveMail(&dir, mail);
    dir = *receiver + "/in/" + *mail->data;
    saveMail(&dir, mail);
}

Node *FileOrganizer::constructNewMail(string* sender, string* receiver, string* subject, string* message) {
    Node* newMail = new Node();
    newMail->data = new string(to_string(randomInt(1000000,9999999)));
    newMail->next = nullptr;
    pushList(new string("Sender: "), newMail);
    pushList(sender, newMail);
    pushList(new string("\nReceiver: "), newMail);
    pushList(receiver, newMail);
    pushList(new string("\nSubject: "), newMail);
    pushList(subject, newMail);
    pushList(new string("\nMessage:\n"), newMail);
    pushList(message, newMail);
    return newMail;
}




// Logic Helper Function
string* FileOrganizer::extractNextSubstring(int *thisDivider, int *nextDivider, string *command) {
    try {
        *thisDivider += *nextDivider+2;
        *nextDivider = (int)command->substr(*thisDivider, command->size()).find("§");
        return new string(command->substr(*thisDivider, *nextDivider));
    } catch(...) {
        return nullptr;
    }
}

string* FileOrganizer::extractRemainingSubstring(int *thisDivider, int *nextDivider, string *command) {
    if (thisDivider == nullptr || nextDivider == nullptr) return new string();

    string message = string();
    string* part = extractNextSubstring(thisDivider, nextDivider, command);
    while (strcmp(part->c_str(), "%") != 0) {
        message += *part;
        message += "\n";
        part = extractNextSubstring(thisDivider, nextDivider, command);
    }
    delete(part);
    return new string(message);
}


bool FileOrganizer::deleteMail(string* name, string* mesNum) {
    if (name == nullptr || mesNum == nullptr || mailSpoolDir == nullptr) return false;

    fs::path pathIn{*mailSpoolDir + *name + "/in/" + *mesNum};
    fs::path pathOut{*mailSpoolDir + *name + "/out/" + *mesNum};
    if (fs::exists(pathIn)) {
        return fs::remove(pathIn) > 0;
    } else if (fs::exists(pathOut)) {
        return fs::remove(pathOut) > 0;
    }
    return false;
}


// STACK FUNCTIONS
void FileOrganizer::pushStack(string* command, string* answer) {
    Node* temp = new Node();

    temp->data = command;
    temp->answer = answer;

    // MUTEX LOCK (unlock automatically, after running out of scope)
    const lock_guard<mutex> lock(pushMutex);
    temp->next = headQueueCommands;
    headQueueCommands = temp;
}

bool FileOrganizer::isStackEmpty() {
    return headQueueCommands == nullptr;
}

Node* FileOrganizer::popStack() {
    Node* temp;

    //    MUTEX LOCK
    const lock_guard<mutex> lock(popMutex);

    // Check for stack underflow
    if (isStackEmpty()) {
        printf("nothing in queue to pop.\n");
        return nullptr;
    }
    else {
        temp = headQueueCommands;
        if (headQueueCommands->next == nullptr) { headQueueCommands = nullptr; }
        else { headQueueCommands = headQueueCommands->next; }

        return temp;
    }
}

void FileOrganizer::pushList(string* str, Node* head) {
    Node* temp = new Node();

    temp->data = str;
    temp->next = nullptr;

    if (head == nullptr) {
        return;
    } else {
        Node* last = getLast(head);
        last->next = temp;
    }
}

Node *FileOrganizer::getLast(Node *head) {
    Node* temp = head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    return temp;
}

