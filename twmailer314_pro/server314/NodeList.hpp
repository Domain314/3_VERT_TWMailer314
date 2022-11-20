#ifndef TWMAILER314_BASIC_NODELIST_HPP
#define TWMAILER314_BASIC_NODELIST_HPP

#include "ComSocket.hpp"

typedef struct ConNode {
    ComSocket* com_socket = nullptr;
    ConNode* next = nullptr;
} ConNode;

//ConNode* createNode(int abortRequested, int communication_socket, int port);
//ConNode* createNode(int abortRequested, int communication_socket, int port, ConNode* next);
//ConNode* getNodeByPort(ConNode* head, int port);
//ConNode* getPrevNode(ConNode* head, ConNode* node);
//ConNode* getLastNode(ConNode* head);
//ConNode* pushNodeStart(ConNode* head, ConNode* node);
//ConNode* pushNodeEnd(ConNode* head, ConNode* node);
//void deleteNode(ConNode* head, ConNode* node);

inline ConNode* createNode(uint port) {
    ConNode* node = new ConNode();
    return node;
}

inline ConNode* getNodeByPort(ConNode* head, int port) {
    ConNode* temp = head;
    while (temp->com_socket->port != port && temp->next != nullptr) {
        temp = temp->next;
    }
    if (temp->com_socket->port == port) return temp;
    else return nullptr;
}

inline ConNode* getNodeByIndex(ConNode* head, int index) {
    ConNode* temp = head;
    while (index > 0 && temp->next != nullptr) {
        temp = temp->next;
        index--;
    }
    if (temp->next == nullptr && index > 0) return nullptr;
    else return temp;
}

inline ConNode* getPrevNode(ConNode* head, ConNode* node) {
    ConNode* temp = head;
    while(temp->next->next != nullptr && temp->next->com_socket->port == node->com_socket->port) {
        temp = temp->next;
    }
    if (temp->next->com_socket->port == node->com_socket->port) return temp;
    else return nullptr;
}

inline ConNode* getLastNode(ConNode* head) {
    ConNode* temp = head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    return temp;
}

inline ConNode* pushNodeStart(ConNode* head, ConNode* node) {
    return node->next = head;
}

inline ConNode* pushNodeEnd(ConNode* head, ConNode* node) {
    return getLastNode(head)->next = node;
}

inline void deleteNode(ConNode* head, ConNode* node) {
    ConNode* prevNode = getPrevNode(head, node);
    prevNode->next = node->next;
    delete(node);
}



#endif //TWMAILER314_BASIC_NODELIST_HPP
