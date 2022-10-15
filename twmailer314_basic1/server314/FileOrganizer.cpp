
#include "FileOrganizer.hpp"

FileOrganizer::FileOrganizer() {

}

bool FileOrganizer::addCommandToQueue(string *command) {
    try {
        pushStack(command);
//        queueCommands->insert(queueCommands->end(),*command);
        queueCounter++;
        if (!isSearching) {
            initSearch();
        }
    } catch (...) {
        printf("Error adding to queue");
        return false;
    }
}

void FileOrganizer::initSearch() {
    isSearching = true;
    while(headQueueCommands != nullptr) {
        try {
            digestCommand(popStack());
            queueCounter--;
        } catch (...) {
            printf("Unknown Error while searching.\n");
        }

    }
    isSearching = false;
}

void FileOrganizer::digestCommand(string *command) {
    if (command == nullptr) { printf("command empty\n");return; }
    printf("command: %s\n", command->c_str());

    switch (command->at(0)) {
        case 'S':
            if (strcmp(command->substr(0, 4).c_str(), "SEND") == 0) {
                printf("Send!\n");
                sendLogic(command);
                break;
            }
        case 'L':
            if (strcmp(command->substr(0, 4).c_str(), "LIST") == 0) {
                printf("List!\n");
                listLogic();
                break;
            }
        case 'R':
            if (strcmp(command->substr(0, 4).c_str(), "READ") == 0) {
                printf("Read!\n");
                readLogic();
                break;
            }
        case 'D':
            if (strcmp(command->substr(0, 3).c_str(), "DEL") == 0) {
                printf("Del!\n");
                delLogic();
                break;
            }
        case 'Q':
        case 'q':
            if (strcmp(command->substr(0, 4).c_str(), "QUIT") == 0 || strcmp(command->substr(0, 4).c_str(), "QUIT") == 0) {
                printf("Quit now.\n");
                quitLogic();
                return;
                break;
            }
        default:
            printf("Command not Found.\n");
//            printHelp();
            return;
    }
}


// Command Logic
void FileOrganizer::sendLogic(string* command) {
    printf("com: %s\n", command->c_str());
    printf("%d\n", command->find("§"));
    int thisDivider = 0;
    int nextDivider = command->substr(thisDivider, command->size()).find("§");
    string subString = command->substr(thisDivider, nextDivider);
    printf("%d:%d = %s\n",thisDivider, nextDivider, subString.c_str());

    string* sender, *receiver, *subject, *message;
    sender = extractNextSubstring(&thisDivider, &nextDivider, command);
    printf("sender: %s\n", sender->c_str());
    receiver = extractNextSubstring(&thisDivider, &nextDivider, command);
    printf("\nreceiver: %s\n", receiver->c_str());
    subject = extractNextSubstring(&thisDivider, &nextDivider, command);
    printf("\nsubject: %s\n", subject->c_str());
    message = extractRemainingSubstring(&thisDivider, &nextDivider, command);
    printf("\nmessage: %s\n", message->c_str());

    initUserDir(sender);
    initUserDir(receiver);
    initNewMail(sender, receiver, subject, message);
}

void FileOrganizer::listLogic() {

}

void FileOrganizer::readLogic() {

}

void FileOrganizer::delLogic() {

}

void FileOrganizer::quitLogic() {

}

// Filesystem WRITE
void FileOrganizer::initUserDir(string *name) {
    createUserDir(new string(*name + "/in"));
    createUserDir(new string(*name + "/out"));
}

void FileOrganizer::createUserDir(string *dir) {
    fs::path path{("./database/" + *dir)};
    fs::create_directories(path);
    delete(dir);
}

void FileOrganizer::saveMail(string* dir, Node* mail) {
    fs::path path{("./database/" + *dir)};
    std::ofstream ofs(path);
    Node* temp = mail->next;
    while (temp != nullptr) {
        ofs << *temp->data;
        temp = temp->next;
    }
    ofs.close();
}



void FileOrganizer::initNewMail(string* sender, string* receiver, string* subject, string* message) {
    Node* mail = constructNewMail(sender, receiver, subject, message);
    string dir = *sender + "/out/" + *mail->data;
    saveMail(&dir, mail);
    dir = *receiver + "/in/" + *mail->data;
    saveMail(&dir, mail);
}

Node *FileOrganizer::constructNewMail(string* sender, string* receiver, string* subject, string* message) {
    Node* newMail;
    newMail->data = new string(to_string(randomInt(1000000,9999999)));
    newMail->next = nullptr;
    pushList(new string("Sender: "), newMail);
    pushList(sender, newMail);
    pushList(new string("Receiver: "), newMail);
    pushList(receiver, newMail);
    pushList(new string("Subject: "), newMail);
    pushList(subject, newMail);
    pushList(new string("Message:\n"), newMail);
    pushList(message, newMail);
    return newMail;
}




// Logic Helper Function
string* FileOrganizer::extractNextSubstring(int *thisDivider, int *nextDivider, string *command) {
    try {
        *thisDivider += *nextDivider+2;
        *nextDivider = command->substr(*thisDivider, command->size()).find("§");
        return new string(command->substr(*thisDivider, *nextDivider));
    } catch(...) {
        return nullptr;
    }

}

string* FileOrganizer::extractRemainingSubstring(int *thisDivider, int *nextDivider, string *command) {
    string message = string();
    string* part = extractNextSubstring(thisDivider, nextDivider, command);
    printf("mes start: %s\n", part->c_str());
    while (strcmp(part->c_str(), "%") != 0 && part != nullptr) {
        printf("%s\n", part->c_str());
        message += *part;
        message += "\n";
        part = extractNextSubstring(thisDivider, nextDivider, command);
    }
    delete(part);
    return new string(message);
}



// STACK FUNCTIONS
void FileOrganizer::pushStack(string* command) {
    Node* temp = new Node();
    if (!temp) { printf("Stack Overflow"); return; }

    temp->data = command;

    // MUTEX LOCK (unlock automatically, after running out of scope)
    const lock_guard<mutex> lock(pushMutex);
    temp->next = headQueueCommands;
    headQueueCommands = temp;
}

bool FileOrganizer::isStackEmpty() {
    return headQueueCommands == nullptr;
}

string* FileOrganizer::popStack() {
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

        string* command = new string(*temp->data);

        free(temp);
        return command;
    }
}

void FileOrganizer::pushList(string* str, Node* head) {
    Node* temp = new Node();
    if (temp == nullptr) { printf("Stack Overflow"); return; }

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















