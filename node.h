#define _CRT_SECURE_NO_WARNINGS
#ifndef _NODE_H
#define _NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*This struct countain the the name, sourceAddress and the type of the label*/
typedef struct Node {
    char name[80];
    int sourceAddress;
    char type[24];
    struct Node* next;
}Node;

/*This struct countain the the name, sourceAddress and the type of the label*/
typedef struct externNode {
    char name[80];
    int sourceAddress;
    struct externNode* next; 
}externNode;

/*This struct countain the address of the head and the tail of the label*/
typedef struct List {
    int count;
    Node* head;
    Node* tail;
}List;

/*This struct countain the address of the head and the tail of the label*/
typedef struct externList {
    int count;
    externNode* head;
    externNode* tail;
}externList;

void addToEnd(char*, int*, char*, List*);
void addToExtEnd(char*, int*, externList*);
#endif 
