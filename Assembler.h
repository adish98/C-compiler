#define _CRT_SECURE_NO_WARNINGS
#ifndef _ASSMBLER_H
#define _ASSMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "preAssembler.h"
#include "Assembler.h"
#include "node.h"

/* Structure representing a row in the chart*/
typedef struct {
    char functionName[20];
    char opcode[20];
} ChartRow;

/* להכניס פה את הכותרות של הפונקציות*/
void Assembler(char* fileName, List* symbolTable);
void getFirstWord(const char* line, char* firstWord);
void getSecondWord(const char* line, char* secondWord);
void removeLastChar(char* inputString);
void labelValidness(char* labelName);
int isLabel(char* line);
void addToSymbolTable(char* labelName, List* symbolTable, int* value, char* type);
void isDataStroage(char* line, int* num);
int exist(char* labelName, List* symbolTable);
void findIndexAfterStr(char* line, int* index, char* str);
int countChar(char* line, char ch);
void encodeCharToBin(char ch, char*** dataArray, int* DC);
void encodeStringData(char* line, char*** dataArray, int* DC);
bool hasMoreThanOneComma(char* line);
bool acceptableChar(char* line);
void encodeIntToBin(int value, char*** dataArray, int* DC);
int isMissingComma(char* line, int* index);
void encodeData(char* line, char*** dataArray, int* DC);
void isExternOrEntry(char* line, int* num);
void addExternLabels(char* line, List* symbolTable, externList* extList);
void removeFirstWord(char* line);
void encodeInstruction(char* line, char*** instArray, int* size, int* L);
void OperandValidness(char* str, int* num);
void pushToInstArray(char* word, char*** instArray, int* size);
void encodeFirstInfoWord(char* line, int type1, int type2, char*** instArray, int* size);
void decimalToBinary(int num, int length, char* binaryString);
void encodeNumber(char* str, char*** instArray, int* IC, int type);
void encodeRegister(char* arg1, char* arg2, char*** instArray, int* IC);
void encodeGroup1(char* line, char*** instArray, int* size, int* L);
void encodeGroup2(char* line, char*** instArray, int* size, int* L);
void encodeGroup3(char* line, char*** instArray, int* size, int* L);
void cmp(char* line, char*** instArray, int* size, int* L);
void prn(char* line, char*** instArray, int* size, int* L);
void lea(char* line, char*** instArray, int* size, int* L);
void updateSymbolTable(List* symbolTable, int IC);
void addEntryLabels(char* line, List* symbolTable);
void checkType(char* arg, int* num);
void getSource(int* num, char* labelName, List* symbolTable, int* isExtern);
void updateLabelG1(char* line, List* symbolTable, char*** instArray, int* IC, externList* extList);
void updateLabelG2(char* line, List* symbolTable, char*** instArray, int* IC, externList* extList);
void encodeLabel(char* line, List* symbolTable, char*** instArray, int* IC, externList* extList);
void number_to_base64(unsigned int num, char* base64, int num_digits);
int binary_to_decimal(const char* binary);
void freeList(List* list);
void freeExtList(externList* list);
void free_arraysAssembler(char*** macroNames, char*** macroContent);

#endif 
