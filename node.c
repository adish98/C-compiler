#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

/*
This function gets a label's name, DC, character array that represent the label's type and a pointer to a list.
The function add those parameters to the end of the list.
*/
void addToEnd(char* labelName, int* value, char* type, List* list)
{
    Node* newNode = (Node*)malloc(sizeof(Node)); /* Allocate memory for a new Node*/
    if (newNode == NULL) {
        /* Memory allocation failed*/
        printf("Memory allocation failed.\n");
        exit(1);
    }

    /* Initialize the fields of the new Node*/
    strncpy(newNode->name, labelName, 79); /* Copy labelName into newNode->name*/
    newNode->name[79] = '\0'; /* Ensure null-termination*/
    newNode->sourceAddress = *value;
    strncpy(newNode->type, type, 24);
    newNode->type[23] = '\0';
    newNode->next = NULL;

    if (list->head == NULL) {
        /* If the list is empty, make the new node both head and tail*/
        list->head = newNode;
        list->tail = newNode;
    }
    else {
        /* Set the next pointer of the current tail node to the new node*/
        list->tail->next = newNode;
        /* Update the tail pointer to the new node*/
        list->tail = newNode;
    }

    /* Increase the count of the list*/
    list->count++;
}/*checks and ready*/

void addToExtEnd(char* labelName, int* value, externList* list)
{
    externNode* newNode = (externNode*)malloc(sizeof(externNode)); /* Allocate memory for a new Node*/
    if (newNode == NULL) {
        /* Memory allocation failed*/
        printf("Memory allocation failed.\n");
        exit(1);
    }

    /* Initialize the fields of the new Node*/
    strncpy(newNode->name, labelName, 79); /* Copy labelName into newNode->name*/
    newNode->name[79] = '\0'; /* Ensure null-termination*/
    newNode->sourceAddress = *value-1;
    newNode->next = NULL;

    if (list->head == NULL) {
        /* If the list is empty, make the new node both head and tail*/
        list->head = newNode;
        list->tail = newNode;
    }
    else {
        /* Set the next pointer of the current tail node to the new node*/
        list->tail->next = newNode;
        /* Update the tail pointer to the new node*/
        list->tail = newNode;
    }

    /* Increase the count of the list*/
    list->count++;
}/*checks and ready*/
