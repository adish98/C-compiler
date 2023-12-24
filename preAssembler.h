#ifndef _PREASSEMBLER_H
#define _PREASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

int isLineBlank(const char*);
int isCommant(const char*);
int getLine(FILE* , char*, int);
int isEndMacro(const char* line);
int isStartMacro(const char* line);
void increaseMcroConByOne(char*** array);
void insertMacroName(char***, char*);
int isExistingMacro(char*** macroNames, const char* line);
void insertLineToFile(FILE*, char*);
void insertMacroToFile(FILE*, char*);
void addLineToArray(char***, char*);
char* getMacroContent(char**, int);
void free_arrays(char*** macroNames, char*** macroContent);
int preAssembler(FILE*, char*);

#endif 
