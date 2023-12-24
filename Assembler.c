#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "preAssembler.h"
#include "Assembler.h"
#include "node.h"

/*Global variables decleration*/
int isError = 0;
int lineCounter = 1;
int labelFlag = 0;
int offSet = 100;

/* Define the chart as an array of ChartRow structures*/
ChartRow opcodes[] = {
    {"mov", "0000"},
    {"cmp", "0001"},
    {"add", "0010"},
    {"sub", "0011"},
    {"not", "0100"},
    {"clr", "0101"},
    {"lea", "0110"},
    {"inc", "0111"},
    {"dec", "1000"},
    {"jmp", "1001"},
    {"bne", "1010"},
    {"red", "1011"},
    {"prn", "1100"},
    {"jsr", "1101"},
    {"rts", "1110"},
    {"stop", "1111"}
};
const char* words[] = { "Entry", "extern", "Extern", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7" };

char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
This function gets a line and a pointer to a character array (firstWord),
Then updates the 'firstWord' address to the first word in the line.
*/
void getFirstWord(const char* line, char* firstWord) {
    if (firstWord != NULL) {
        if (sscanf(line, " %79s", firstWord) != 1) {
            /* Handle error or no first word found*/
            /* For example, you can strcpy an error message to firstword: */
            firstWord[0] = '\0'; /* Assign an empty string to firstWord*/
        }
    }
}


/*
This function gets a line and a pointer to a character array (secondWord),
Then updates the 'secondWord' address to the first word in the line.
*/
void getSecondWord(const char* line, char* secondWord) {
    if (secondWord != NULL) {
        if (sscanf(line, "%*s %79s", secondWord) != 1) {
            /* Handle error or no second word found */
            secondWord[0] = '\0'; /* Assign an empty string to secondWord*/
        }
    }
}

/*
This function removes the last character from the inputString.
*/
void removeLastChar(char* inputString)
{
    int length = (int)strlen(inputString);
    if (length > 0) {
        inputString[length - 1] = '\0';
    }
} /*checked and ready*/

/*
This function checks for the validness of the label's name.
*/
void labelValidness(char* labelName)
{
    int index = 0;
    int count = 0;
    int numOpcodes = 0;
    int i = 0;
    int found = 0; /* Flag to indicate if the labelName is found*/
    if (strlen(labelName) == 0)
    {
        printf("Error in line %d: invlid label name - empty label! (only ':')\n", lineCounter);
        isError = 1;
        return;
    }
    if (!isalpha(*labelName))
    {
        printf("Error in line %d: invlid label name - should start with letter!\n", lineCounter);
        isError = 1;
    }
    while (labelName[index] != '\0')
    {
        if (!isalpha(labelName[index]) && !isdigit(labelName[index]))
        {
            count++;
        }
        index++;
    }
    if (count != 0) /*Too many colons error*/
    {
        printf("Error in line %d: invlid label name - should be only letters and numbers!\n", lineCounter);
        isError = 1;
    }
    numOpcodes = sizeof(opcodes) / sizeof(opcodes[0]);
    while (i < numOpcodes && strcmp(labelName, opcodes[i].functionName) != 0) {
        i++;
    }
    if (i < numOpcodes) /*Label name is like a function name error*/
    {
        printf("Error in line %d: invlid label name - label's name is a key word in assembly!\n", lineCounter);
        isError = 1;
        return;
    }

    /* Check if labelName matches any of the specified words*/
    for (i = 0; i < sizeof(words) / sizeof(words[0]); i++) {
        if (strcmp(labelName, words[i]) == 0) {
            found = 1;
            break; /* No need to continue searching*/
        }
    }
    if (found == 1)
    {
        printf("Error in line %d: invlid label name - label's name is a key word in assembly!\n", lineCounter);
        isError = 1;
        return;
    }
}

/*
This function checks if the first word in the line is a label.
If so then the function calls the 'labelValidness' function.
returns 0 if the first word is a label else returns 1.
*/
int isLabel(char* line)
{
    char* firstWord = (char*)malloc(80 * sizeof(char)); 
    int len = 0;
    if (firstWord == NULL) {
        printf("Memory allocation failed\n");
        exit(0);
    }
    getFirstWord(line, firstWord); 
    len = (int)strlen(firstWord);
    if (firstWord[len - 1] == ':') /*Meaning label definition*/
    {
        removeLastChar(firstWord);
        labelValidness(firstWord);
        free(firstWord);
        return 0;
    }
    free(firstWord);
    return 1;
}

/*
This function gets a label's name, symbol table and memory value.
The function checks whether the label name is not already in the symbol table (using 'exist' function),
if yes an error will rize.
*/
void addToSymbolTable(char* labelName, List* symbolTable, int* value, char* type)
{
    /*Checks if the label is already exist in the chart*/
    if (exist(labelName, symbolTable) == 1) {
        printf("Error in line %d: label already exists\n", lineCounter);
        isError = 1;
    }
    else {
        addToEnd(labelName, value, type, symbolTable);
    }
}

/*
This function gets a line and a pointer to an integer (num).
The function checks if the line is a data storage line (meaning if there is .data or .string in the line),
then update the value in num respectively.
0 -> ".string/.String".
1 -> ".data/.Data".
-1 -> if none of the above.
*/
void isDataStroage(char* line, int* num)
{
    char s1[80];
    char s2[80];
    int numScanned;
    s1[79] = '\0';
    s2[79] = '\0';
    *num = -1;
    if (labelFlag == 1)
    {
        numScanned = sscanf(line, "%s %s", s1, s2);
        if (numScanned == 2)
        {
            if (strcmp(s2, ".string") == 0 || strcmp(s2, ".String") == 0)
            {
                *num = 0;
            }
            if (strcmp(s2, ".data") == 0 || strcmp(s2, ".Data") == 0)
            {
                *num = 1;
            }
        }
        else { /*Meaning there is an error*/
            printf("Error in line %d: Missing text after label!\n", lineCounter);
            isError = 1;
        }
    }
    else {
        numScanned = sscanf(line, "%s", s1);
        if (strcmp(s1, ".string") == 0 || strcmp(s1, ".String") == 0)
        {
            *num = 0;
        }
        if (strcmp(s1, ".data") == 0 || strcmp(s1, ".Data") == 0)
        {
            *num = 1;
        }
    }
}

/*
This function checks if a label' name is already exist in the symbol table.
returns 1 if its already exist, else returns 0.
*/
int exist(char* labelName, List* symbolTable)
{
    Node* tempNode = symbolTable->head;
    while (tempNode != NULL)
    {
        if (strcmp(tempNode->name, labelName) == 0)
            return 1;
        tempNode = tempNode->next;
    }
    return 0;
}/*checks and ready*/

/*
This function will update the index to the index after the specific string (str)
*/

void findIndexAfterStr(char* line, int* index, char* str) {
    char* found = strstr(line, str);
    if (found != NULL) {
        *index = (int)(found - line + strlen(str));
        while (line[*index] == ' ' && line[*index] != '\0')
        {
            (*index)++;
        }
    }
    else
    {
        *index = -1;
    }
    if (line[*index] == '\0')
    {
        *index = -1;
    }
}

/*
This function counts how many 'ch' there is in 'line'.
*/
int countChar(char* line, char ch)
{
    int count = 0;
    int i = 0;
    while (line[i] != '\0')
    {
        if (line[i] == ch)
        {
            count++;
        }
        i++;
    }
    return count;
}

/*
This function gets a char (ch), a data array (dataArray) and a data counter (DC).
The function convert the char to it's binary form and pushes it to the data array and update DC (adds 1).
*/
void encodeCharToBin(char ch, char*** dataArray, int* DC) {
    int i = 0;
    char** tempArray = NULL;
    /* Convert the character 'ch' to its ASCII value*/
    int asciiValue = (int)ch;
    /* Allocate memory for the binary string (12 characters + 1 for null-termination)*/
    char* binaryString = (char*)malloc(13 * sizeof(char));
    if (binaryString == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    /* Convert the ASCII value to a binary string and store it in binaryString*/
    for (i = 11; i >= 0; i--) {
        binaryString[i] = (asciiValue % 2) + '0';
        asciiValue /= 2;
    }
    binaryString[12] = '\0'; /* Null-terminate the binary string*/
    /* Allocate memory for a new pointer in dataArray to store the binaryString*/
    tempArray = (char**)realloc(*dataArray, (*DC + 2) * sizeof(char*));
    if (tempArray == NULL) {
        printf("Memory allocation failed.\n");
        free(binaryString);
        exit(1);
    }
    (*dataArray) = tempArray;
    (*dataArray)[*DC] = binaryString;
    (*dataArray)[*DC + 1] = NULL;
    /* Increase the DC for the next insertion*/
    (*DC)++;
}


/*
This function gets a character array (line), a data array (data array) and a data counter (DC).
The function gets a data storage line (.string type) and checks for any problems in the line,
if the line is valid the function will get the data and encode it to binary form.
*/
void encodeStringData(char* line, char*** dataArray, int* DC)
{
    /* STR: .string "adsf  sfv" "dfvev"*/
    /* .string "sdvefvb"*/
    int index = 0;
    int len = 0;
    char current;
    int currentError = 0;
    findIndexAfterStr(line, &index, ".string");
    len = (int)strlen(line);
    if (index >= len) {
        printf("Error in line %d: No text after \".string\"!\n", lineCounter);
        isError = 1;
        currentError = 1;
    }
    if (line[index] != '"')
    {
        printf("Error in line %d: Missing apostrophes!\n", lineCounter);
        isError = 1;
        currentError = 1;
    }
    else if (countChar(line, '"') == 1)
    {
        printf("Error in line %d: Missing apostrophes!\n", lineCounter);
        isError = 1;
        currentError = 1;
    }
    if (countChar(line, '"') > 2) {
        printf("Error in line %d: Too many apostrophes!\n", lineCounter);
        isError = 1;
        currentError = 1;
    }
    if (currentError == 1)
    {
        return;
    }
    index++;
    while ((current = line[index]) != '"') {
        encodeCharToBin(current, dataArray, DC);
        index++;
    }
    index++;
    while (line[index] != '\0' && line[index] != '\n')
    {
        if (line[index] != ' ')
        {
            printf("Error in line %d: Unnecessary text after string!\n", lineCounter);
            isError = 1;
            break;
        }
        index++;
    }
    encodeCharToBin('\0', dataArray, DC);
}

/*
This function gets a line and checks if there are more then one comma next to each other.
The function returns:
true  - if there is more than one comma next to each other.
false - if there is more than one comma next to each other.
*/
bool hasMoreThanOneComma(char* line) {
    int commaCount = 0;
    while (*line) {
        if (*line == ',') {
            commaCount++;
        }
        if (commaCount > 1) {
            return true;
        }
        if ((*line >= '0' && *line <= '9') || (*line >= 'A' && *line <= 'z')) {
            commaCount = 0;
        }
        line++;
    }
    return false;
}


/*
This function checks if the line consist only numbers, commas, spaces, plus signa and minus sign.
The function returns:
true  - if the line has not acceptable chars.
false - if the line has only acceptable chars.
*/
bool acceptableChar(char* line) {
    int index = 0;
    findIndexAfterStr(line, &index, ".data");
    while (line[index] != '\0' && line[index] != '\n') {
        if ((line[index] < '0' || line[index] > '9') && line[index] != ',' && line[index] != ' ' && line[index] != '+' && line[index] != '-') {
            return true;
        }
        index++;
    }
    return false;
}

/*
This function gets a integer (value), a data array (dataArray) and a data counter (DC).
The function convert the integer to its binary form and then pushes it to the data array and updates DC (adds 1).
*/
void encodeIntToBin(int value, char*** dataArray, int* DC) {
    int i = 0;
    char** tempArray = NULL;
    /* Apply a mask to get the rightmost 12 bits */
    int mask = 0xFFF; /* 0xFFF is a 12-bit mask (binary: 1111 1111 1111) */
    int maskedValue = value & mask;
    /* Allocate memory for the binary string (12 characters + 1 for null-termination) */
    char* binaryString = (char*)malloc(13 * sizeof(char));
    if (binaryString == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    /* Convert the maskedValue to a binary string and store it in binaryString */
    for (i = 11; i >= 0; i--) {
        binaryString[i] = (maskedValue % 2) + '0';
        maskedValue /= 2;
    }
    binaryString[12] = '\0'; /* Null-terminate the binary string */
    /* Allocate memory for a new pointer in dataArray to store the binaryString */
    tempArray = (char**)realloc(*dataArray, (*DC + 2) * sizeof(char*));
    if (tempArray == NULL) {
        printf("Memory allocation failed.\n");
        free(binaryString);
        exit(1);
    }
    (*dataArray) = tempArray;
    (*dataArray)[*DC] = binaryString;
    (*dataArray)[*DC + 1] = NULL;
    /* Increase the DC for the next insertion */
    (*DC)++;
}


/*
This function gets a line and a index within that line.
The function checks if there is a missing comma between two operands in the line (after the giving index).
It returns an integer value: 1 if a missing comma is found, and 0 otherwise.
*/
int isMissingComma(char* line, int* index)
{
    int tempIndex = *index;
    int temp = 0;
    int spaceActive = 0;
    int whilecheck = 0;
    while (line[tempIndex] != '\0')
    {
        if (line[tempIndex] == ' ')
        {
            spaceActive = 1;
        }
        if (line[tempIndex] == ',')
        {
            while (line[tempIndex + 1] == ' ')
            {
                tempIndex++;
            }
        }

        temp = tempIndex;
        while (line[temp] != '\0' && spaceActive == 1 && line[temp] != '\n')
        {
            /* Check for unexpected characters (operands should be numbers or labels) */
            if ((line[temp] <= '9' && line[temp] >= '0') || ((line[temp] <= 'z' && line[temp] >= 'a') || (line[temp] <= 'Z' && line[temp] >= 'A')))
            {
                isError = 1;
                return 1;
            }
            else if (line[temp] == ',')
            {
                spaceActive = 0;
                while (line[temp + 1] == ' ')
                {
                    temp++;
                }
                break;
            }
            temp++;
            whilecheck = 1;
            while (line[temp] == ' ')
            {
                temp++;
            }
        }
        /* Check if the loop should continue or advance tempIndex */
        if (whilecheck == 1 && line[temp] != '\0' && line[temp] != '\n')
        {
            tempIndex = temp + 1;
            whilecheck = 0;
        }
        else
        {
            tempIndex++;
        }

    }
    return 0;
}

/*
This function gets a line, data array and a data counter.
The function Converts the data information into its binary form and adds the code to the data array.
*/
void encodeData(char* line, char*** dataArray, int* DC)
{
    int index = 0;
    int len = 0;
    int num = 0;
    char digit;
    int digitValue = 0;
    int isNegative = 0; 
    len = (int)strlen(line);
    findIndexAfterStr(line, &index, ".data");
    len = (int)strlen(line);
    if (index >= len) {
        printf("Error in line %d: No numbers after \".data\"!\n", lineCounter);
        isError = 1;
    }
    else
    {
        if (line[index] == ',')
        {
            printf("Error in line %d: illegal comma before first number!\n", lineCounter);
            isError = 1;
        }
        if (line[len - 1] == ',')
        {
            printf("Error in line %d: illegal comma after last number!\n", lineCounter);
            isError = 1;
        }
        if (hasMoreThanOneComma(line))
        {
            printf("Error in line %d: Too many commas between numbers!\n", lineCounter);
            isError = 1;
        }
        if (acceptableChar(line))
        {
            printf("Error in line %d: Illegal characters in data!\n", lineCounter);
            isError = 1;
        }
        if (isMissingComma(line, &index) == 1)
        {
            printf("Error in line %d: Missing comma!\n", lineCounter);
            isError = 1;
        }
        if (isError == 1)
        {
            return;
        }
    }
    while (line[index] != '\0')
    {
        if (line[index] == ',' || line[index] == '\n')
        {
            if (isNegative == 1)
            {
                num = num * (-1);
            }
            encodeIntToBin(num, dataArray, DC);
            num = 0;
            isNegative = 0;
        }
        else if (line[index] >= '0' && line[index] <= '9')
        {

            digit = line[index];
            digitValue = digit - '0'; /* Convert the character to its integer value*/
            num = num * 10 + digitValue;
        }
        if (line[index] == '-')
        {
            isNegative = 1;
        }
        index++;
    }
    if (line[index - 1] != '\n')
    {
        encodeIntToBin(num, dataArray, DC);
    }
}

/*
This function gets a line and a pointer to an integer.
The function checks if the line is an .entry or .extern.
The function updates num respectively.
*/
void isExternOrEntry(char* line, int* num)
{
    char str[80];
    getFirstWord(line, str);
    if (strcmp(str, ".entry") == 0 || strcmp(str, ".Entry") == 0)
    {
        *num = 0;
    }
    else if (strcmp(str, ".extern") == 0 || strcmp(str, ".Extern") == 0)
    {
        *num = 1;
    }
    else {
        *num = -1;
    }
}

/*
This function gets an extern line type then gets the labels in that line and put them in the symbol table.
*/
void addExternLabels(char* line, List* symbolTable, externList* extList)
{
    
    int index = 0;
    int newLabelIndex = 0;
    int value = 0;
    char str[80];
    int len;
    char* newLabel = (char*)malloc(32 * sizeof(char));
    getFirstWord(line, str);
    findIndexAfterStr(line, &index, str);
    len = (int)strlen(line);
    if (index >= len) /* checks if there is no label */
    {
        printf("Error in line %d: No labels after \".extern\"!\n", lineCounter);
        isError = 1;
    }
    if (line[len - 1] == ',') /* last char is comma*/
    {
        printf("Error in line %d: No label after comma!\n", lineCounter);
        isError = 1;
    }
    if (line[index] == ',') /* no labl before comma */
    {
        printf("Error in line %d: No label after comma!\n", lineCounter);
        isError = 1;
    }
    if (hasMoreThanOneComma(line))/*checks if there is more than one comma*/
    {
        printf("Error in line %d: Too many commas between numbers!\n", lineCounter);
        isError = 1;
    }
    if (isMissingComma(line, &index) == 1)/*checks if there is missing comma between labels*/
    {
        printf("Error in line %d: Missing comma!\n", lineCounter);
        isError = 1;
    }
    if (isError == 1)
    {
        return;
    }
    else
    { 
        while (line[index] != '\0' && line[index] != '\n')
        {
            newLabel = (char*)calloc(sizeof(newLabel), sizeof(char));
            if (newLabel == NULL)
            {
                printf("Memory allocation failed!");
                exit(1);
            }
            while (line[index] != ',' && line[index] != '\0' && line[index] != '\n')
            {
                if (line[index] != ' ')
                {
                    newLabel[newLabelIndex] = line[index];
                    newLabelIndex++;
                }
                index++;
            }
            /*.extern L,  h  , g,a,s,v*/
            if (newLabel)
            {
                labelValidness(newLabel);
                newLabel[newLabelIndex] = '\0';
                addToSymbolTable(newLabel, symbolTable, &value, "external");
                index++;
                newLabelIndex = 0;
            }
        }
    }
}

/*
This function remove the first word in the line
*/
void removeFirstWord(char* line) {
    /* Find the index of the first space or tab character*/
    int firstSpaceIndex = 0;
    int remainingLength = 0;
    while (line[firstSpaceIndex] && !isspace(line[firstSpaceIndex])) {
        firstSpaceIndex++;
    }

    /* If the first character is a space, ignore leading spaces*/
    while (isspace(line[firstSpaceIndex])) {
        firstSpaceIndex++;
    }

    /* Calculate the length of the remaining string (after removing the first word)*/
    remainingLength = (int)strlen(line) - firstSpaceIndex;

    /* Shift the remaining characters to the beginning of the string*/
    memmove(line, line + firstSpaceIndex, remainingLength + 1); /* +1 to include the null terminator*/
}

/*
This function checks for an instruction word and encode the instruction if exist respectively.
*/
void encodeInstruction(char* line, char*** instArray, int* size, int* L)
{
    char str[80];
    if (labelFlag == 1)
    {
        getSecondWord(line, str);
        removeFirstWord(line);

    }
    else {
        getFirstWord(line, str);
    }
    /* Compare the instruction mnemonic with known mnemonics */
    if (strcmp(str, "mov") == 0) {
        encodeGroup1(line, instArray, size, L);
    }
    else if (strcmp(str, "cmp") == 0) {
        cmp(line, instArray, size, L);
    }
    else if (strcmp(str, "add") == 0) {
        encodeGroup1(line, instArray, size, L);
    }
    else if (strcmp(str, "sub") == 0) {
        encodeGroup1(line, instArray, size, L);
    }
    else if (strcmp(str, "not") == 0) {
        encodeGroup2(line, instArray, size, L);
    }
    else if (strcmp(str, "clr") == 0) {
        encodeGroup2(line, instArray, size, L);
    }
    else if (strcmp(str, "lea") == 0) {
        lea(line, instArray, size, L);
    }
    else if (strcmp(str, "inc") == 0) {
        encodeGroup2(line, instArray, size, L);
    }
    else if (strcmp(str, "dec") == 0) {
        encodeGroup2(line, instArray, size, L);
    }
    else if (strcmp(str, "jmp") == 0) {
        encodeGroup2(line, instArray, size, L);
    }
    else if (strcmp(str, "bne") == 0) {
        encodeGroup2(line, instArray, size, L);
    }
    else if (strcmp(str, "red") == 0) {
        encodeGroup2(line, instArray, size, L);
    }
    else if (strcmp(str, "prn") == 0) {
        prn(line, instArray, size, L);
    }
    else if (strcmp(str, "jsr") == 0) {
        encodeGroup2(line, instArray, size, L);
    }
    else if (strcmp(str, "rts") == 0) {
        encodeGroup3(line, instArray, size, L);
    }
    else if (strcmp(str, "stop") == 0) {
        encodeGroup3(line, instArray, size, L);
    }
    else if (strcmp(str, "") != 0) {
        printf("Error in line %d: unknown command name!\n", lineCounter);
        isError = 1;
    }
}

/*
This function gets a string which is a operand and an integer.
The function checks the validness of the operand whether its a register or a label.
The function also update num to be as the Addressing method (שיטת מיעון).
*/
void OperandValidness(char* str, int* num)
{
    if (str[0] == '@') /*Meaning a register*/
    {
        *num = 5;
        if (str[1] != 'r')
        {
            printf("Error in line %d: Unknown register name!\n", lineCounter);
            isError = 1;
        }
        else if (atoi(&str[2]) < 0 || atoi(&str[2]) > 7) {
            printf("Error in line %d: Unknown register name!\n", lineCounter);
            isError = 1;
        }
    }
    else { /*Meaning a label*/
        *num = 3;
        labelValidness(str);
    }
}

/*
This function adds a word to the instArray.
*/
void pushToInstArray(char* word, char*** instArray, int* size)
{
    char** tempArray = NULL;
    (*size)++;
    /* Allocate memory for a new pointer in instArray to store the word */
    tempArray = realloc(*instArray, ((*size + 1) * sizeof(char*)));

    if (tempArray == NULL) {
        printf("Memory allocation failed.\n");
        free(word);
        exit(1);
    }
    (*instArray) = tempArray;
    (*instArray)[*size - 1] = (char*)malloc(13);
    (*instArray)[*size] = NULL;
    if ((*instArray)[*size - 1] == NULL) {
        printf("Memory allocation failed.\n");
        free(word);
        exit(1);
    }
    strcpy((*instArray)[*size - 1], word);
}


/*
This function encode a instruction line accurding to the maman rules, and pushes the line to the instArray and update IC.
for example:
mov -103, @r1 -> 101000000100
*/
void encodeFirstInfoWord(char* line, int type1, int type2, char*** instArray, int* size)
{
    int i = 0;
    char* first = (char*)malloc(sizeof(char));
    char* word = (char*)malloc(13 * sizeof(char));
    if (word == NULL || first == NULL) {
        printf("Memory allocation failed\n");
        exit(0);
    }
    /* A, R, E is set to "00"*/
    word[10] = '0';
    word[11] = '0';
    word[12] = '\0';
    /* mion 1*/
    switch (type1)
    {
    case 1:
        word[0] = '0';
        word[1] = '0';
        word[2] = '1';
        break;
    case 3:
        word[0] = '0';
        word[1] = '1';
        word[2] = '1';
        break;
    case 5:
        word[0] = '1';
        word[1] = '0';
        word[2] = '1';
        break;
    default:
        word[0] = '0';
        word[1] = '0';
        word[2] = '0';
    }
    /* opcode*/
    getFirstWord(line, first);
    while (i < 16)
    {
        if (strcmp(first, opcodes[i].functionName) == 0)
        {
            strncpy(word + 3, opcodes[i].opcode, 4);
            break;
        }
        i++;
    }
    /* mion 2*/
    switch (type2)
    {
    case 1:
        word[7] = '0';
        word[8] = '0';
        word[9] = '1';
        break;
    case 3:
        word[7] = '0';
        word[8] = '1';
        word[9] = '1';
        break;
    case 5:
        word[7] = '1';
        word[8] = '0';
        word[9] = '1';
        break;
    default:
        word[7] = '0';
        word[8] = '0';
        word[9] = '0';
    }
    pushToInstArray(word, instArray, size);
}


/*
Function to convert a decimal number to binary string of fixed 10-bit length
*/
void decimalToBinary(int num, int length, char* binaryString) {
    int i = 0;
    int mask = 1 << (length - 1); /* Start with the leftmost bit (10th bit)*/

    for (i = 0; i < length; i++) {
        binaryString[i] = (num & mask) ? '1' : '0';
        mask >>= 1; /* Move the mask to the right for the next bit*/
    }

    binaryString[10] = '\0'; /* Null-terminate the string*/
}

/*
This function gets a string that represent a number then the function create info word.
Example - "8" -> "000000100000"
*/
void encodeNumber(char* str, char*** instArray, int* IC, int type)
{
    /*num - "8" -> "000000100000" -> push to array and increase IC*/
    char* word = NULL;
    int num = 0;
    char binaryString[11]; /* 10 bits + 1 for null terminator*/
    word = (char*)malloc(13 * sizeof(char)); /* Allocate memory for the instruction*/
    if (word == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    word[12] = '\0';
    num = atoi(str);
    if (num > 1023)
    {
        printf("Error in line %d: Number is too large (expected up to 1023)!\n", lineCounter);
        isError = 1;
    }
    if (word == NULL) {
        printf("Memory allocation failed\n");
        exit(0);
    }
    /* A, R, E*/
    if (type == 0)
    {
        word[10] = '0';
        word[11] = '0';
        word[12] = '\0';
    }
    else if (type == 1) {
        word[10] = '1';
        word[11] = '0';
        word[12] = '\0';
    }
    else
    {
        word[10] = '0';
        word[11] = '1';
        word[12] = '\0';
    }

    /*binaryString[10] = '\0';*/
    decimalToBinary(num, 10, binaryString);
    strncpy(word, binaryString, 10);
    word[12] = '\0';
    if (type == 0)
    {
        pushToInstArray(word, instArray, IC);
    }
    else {
        strcpy((*instArray)[*IC], word);
    }

}

/*This function encode the string that has the register name into binary code*/
void encodeRegister(char* arg1, char* arg2, char*** instArray, int* IC)
{
    int num;
    char temp[6];
    char* word = (char*)malloc(13 * sizeof(char)); /* Allocate memory for the instruction*/
    if (word == NULL) {
        printf("Memory allocation failed\n");
        exit(0);
    }
    /* A, R, E is set to "00"*/
    word[10] = '0';
    word[11] = '0';
    word[12] = '\0';
    if (arg1 != NULL && arg2 == NULL)
    {
        /* 00000 - 00000 - 00*/
        strncpy(word + 5, "00000", 5);
        num = atoi(&arg1[2]);
        decimalToBinary(num, 5, temp);
        strncpy(word, temp, 5);
    }
    else if (arg1 == NULL && arg2 != NULL)
    {
        strncpy(word, "00000", 5);
        num = atoi(&arg2[2]);
        decimalToBinary(num, 5, temp);
        strncpy(word + 5, temp, 5);
    }
    else if (arg1 != NULL && arg2 != NULL)
    {
        num = atoi(&arg1[2]);
        decimalToBinary(num, 5, temp);
        strncpy(word, temp, 5);

        num = atoi(&arg2[2]);
        decimalToBinary(num, 5, temp);
        strncpy(word + 5, temp, 5);

    }
    pushToInstArray(word, instArray, IC);
}


/*
This function encode group 1  which consist of this 3:
mov, add, sub
****here we get a instruction line with out a label.
This function get a line and the isntWord array,
then encode the instruction and put the encoded code to the array.
*/
void encodeGroup1(char* line, char*** instArray, int* size, int* L)
{
    char arg1[50];
    char arg2[50];
    char word1[20];
    char word2[20];
    char funcName[20];
    int type1 = 0;
    int type2 = 0;
    int index = 0;
    int i = 0;
    int dotFlag = 0;
    int numScanned = 0;
    arg2[0] = '~';
    arg1[49] = '\0';
    arg2[49] = '\0';

    getFirstWord(line, funcName);
    findIndexAfterStr(line, &index, funcName);
    if (line[index] == ',')
    {
        printf("Error in line %d: Illegal comma!\n", lineCounter);
        if (countChar(line, ',') == 1)
        {
            printf("Error in line %d: Missing Parameter!\n", lineCounter);
        }
        isError = 1;
        arg1[0] = '\0';
    }
    if (hasMoreThanOneComma(line))/*checks if there is more than one comma*/
    {
        printf("Error in line %d: Too many commas between numbers!\n", lineCounter);
        isError = 1;
    }
    numScanned = sscanf(line, "%s %s", word1, word2);
    if (numScanned == 1)
    {
        printf("Error in line %d: Missing operands!\n", lineCounter);
        isError = 1;
    }
    else
    {
        if (isMissingComma(line, &index) == 1)
        {
            printf("Error in line %d: Missing comma!\n", lineCounter);
            isError = 1;
        }
        else {
            findIndexAfterStr(line, &index, funcName);
            while (line[index] != ',' && line[index] != '\0' && line[index] != '\n')
            {
                if (arg1 != NULL && line[index] == ' ')
                {
                    break;
                }
                if (line[index] != ' ')
                {
                    arg1[i] = line[index];
                    i++;
                    arg1[i] = '\0';
                    if (line[index] == '.')
                    {
                        dotFlag = 1;
                    }
                }
                index++;
            }
            if (line[index] == '\0' && line[index] == '\n')
            {
                printf("Error in line %d: Missing operands!\n", lineCounter);
                isError = 1;
            }
            if ((strcmp(arg1, "0") == 0) || atoi(arg1) != 0) /*Meaning a number*/
            {
                if (dotFlag == 1)
                {
                    printf("Error in line %d: Expexted natural numbers!\n", lineCounter);
                    isError = 1;
                }
                type1 = 1;
            }
            else
            {
                if (arg1[0] != '\0')
                    OperandValidness(arg1, &type1);
            }
        /* Check for unnecessary text after instruction */
            if (countChar(line, ',') > 1)
            {
                printf("Error in line %d: Unnecessary text after instruction!\n", lineCounter);
                isError = 1;
            }
            else if (countChar(line, ',') == 0)
            {
                printf("Error in line %d: Missing operands!\n", lineCounter);
                isError = 1;
            }
            else {
                i = 0;
                findIndexAfterStr(line, &index, ",");
                while (line[index] != '\0' && line[index] != '\n')
                {
                    if (arg2 != NULL && line[index] == ' ')
                    {
                        break;
                    }
                    if (line[index] != ' ')
                    {

                        arg2[i] = line[index];
                        i++;
                        arg2[i + 1] = '\0';
                    }
                    index++;
                }
            /* Check for missing second operand */
                if (arg2[0] == '~')
                {
                    printf("Error in line %d: Missing operands!\n", lineCounter);
                    isError = 1;
                }
                else if ((strcmp(arg2, "0") == 0) || atoi(arg2) != 0)
                {
                    type2 = 1;
                    printf("Error in line %d: Operand shouldn't be a number!\n", lineCounter);
                    isError = 1;
                }
                else {
                    OperandValidness(arg2, &type2);
                }

                if (isError != 1)
                {
                    encodeFirstInfoWord(line, type1, type2, instArray, size);
                    (*L)++;
                    if (type1 == 1 && type2 == 3) /*mov -103, Length*/
                    {
                        encodeNumber(arg1, instArray, size, 0);
                        pushToInstArray("?", instArray, size);
                        (*L) = (*L) + 2;
                    }
                    else if (type1 == 1 && type2 == 5) /*mov -103, @r1*/
                    {
                        encodeNumber(arg1, instArray, size, 0);
                        encodeRegister(NULL, arg2, instArray, size);
                        (*L) = (*L) + 2;
                    }
                    else if (type1 == 3 && type2 == 3) /*mov Length, HELLO*/
                    {
                        pushToInstArray("?", instArray, size);
                        pushToInstArray("?", instArray, size);
                        (*L) = (*L) + 2;
                    }
                    else if (type1 == 3 && type2 == 5) /*mov Length, @r1*/
                    {
                        (*L) = (*L) + 2;
                        pushToInstArray("?", instArray, size);
                        encodeRegister(NULL, arg2, instArray, size);
                    }
                    else if (type1 == 5 && type2 == 3) /*mov @r1, HELLO*/
                    {
                        encodeRegister(arg1, NULL, instArray, size);
                        pushToInstArray("?", instArray, size);
                        (*L) = (*L) + 2;
                    }
                    else if (type1 == 5 && type2 == 5) /*mov @r2, @r1*/
                    {
                        encodeRegister(arg1, arg2, instArray, size);
                        (*L)++;
                    }
                }

            }
        }
    }
}

/*
This function encode group 2  which consist of this 8:
not,clr,inc,dec,jmp,bne,red,jsr
****here we get a instruction line with out a label.
This function get a line and the isntWord array,
then encode the instruction and put the encoded code to the array.
*/

void encodeGroup2(char* line, char*** instArray, int* size, int* L)
{
    char arg1[50];
    int i = 0;
    char word1[20];
    char word2[20];
    char funcName[20];
    int type1 = 0;
    int index = 0;
    int numScanned = 0;
    arg1[49] = '\0';
    word1[19] = '\0';
    word2[19] = '\0';
    funcName[19] = '\0';


    getFirstWord(line, funcName);
    findIndexAfterStr(line, &index, funcName);
    /* Check for illegal comma */
    if (line[index] == ',')
    {
        printf("Error in line %d: Illegal comma!\n", lineCounter);
        isError = 1;
    }
    /* Check for missing operands */
    numScanned = sscanf(line, "%s %s", word1, word2);
    if (numScanned == 1)
    {
        printf("Error in line %d: Missing operands!\n", lineCounter);
        isError = 1;
    }
    else
    {
        findIndexAfterStr(line, &index, funcName);
        while (line[index] != ',' && line[index] != '\0' && line[index] != '\n')
        {

            if (arg1 != NULL && line[index] == ' ')
            {
                break;
            }
            if (line[index] != ' ')
            {
                arg1[i] = line[index];
                i++;
                arg1[i] = '\0';
            }
            index++;
        }
        if ((strcmp(arg1, "0") == 0) || atoi(arg1) != 0) /*Meaning a number*/
        {
            printf("Error in line %d: Incorrect type of operand!\n", lineCounter);
        }
        else
        {
            OperandValidness(arg1, &type1);
        }
        while (line[index] != '\0' && line[index] != '\n')
        {
            if (line[index] != ' ')
            {
                printf("Error in line %d: Unnecessary text after instruction!\n", lineCounter);
                isError = 1;
                break;
            }
            index++;
        }

        if (isError != 1)
        {
            encodeFirstInfoWord(line, -1, type1, instArray, size);
            (*L)++;
            if (type1 == 3) 
            {
                pushToInstArray("?", instArray, size);
                (*L)++;
            }
            else 
            {
                encodeRegister(arg1, NULL, instArray, size);
                (*L)++;
            }
        }
    }
}

/*This function encode group 3  which consist of this 2:
rts,stop
****here we get a instruction line with out a label.
This function get a line and the isntWord array,
then encode the instruction and put the encoded code to the array.
*/

void encodeGroup3(char* line, char*** instArray, int* size, int* L)
{
    char funcName[20];
    int index = 0;
    getFirstWord(line, funcName);
    findIndexAfterStr(line, &index, funcName);
    if (index != -1 && line[index] != '\n')
    {
        printf("Error in line %d: Unnecessary text after instruction!\n", lineCounter);
        isError = 1;
    }
    /* Encode the instruction if there are no errors */
    if (isError != 1)
    {
        encodeFirstInfoWord(line, -1, -1, instArray, size);
        (*L)++;
    }
}

/*
This function deals with the 'cmp' function.
*/
void cmp(char* line, char*** instArray, int* size, int* L)
{
    char arg1[50];
    char arg2[50];
    char word1[20];
    char word2[20];
    char funcName[20];
    int type1 = 0;
    int type2 = 0;
    int index = 0;
    int i = 0;
    int dotFlag = 0;
    int numScanned = 0;
    arg2[0] = '~';
    arg1[49] = '\0';
    arg2[49] = '\0';
    word1[19] = '\0';
    word2[19] = '\0';
    funcName[19] = '\0';

    getFirstWord(line, funcName);
    findIndexAfterStr(line, &index, funcName);
    if (line[index] == ',')
    {
        printf("Error in line %d: Illegal comma!\n", lineCounter);
        if (countChar(line, ',') == 1)
        {
            printf("Error in line %d: Missing Parameter!\n", lineCounter);
        }
        isError = 1;
        arg1[0] = '\0';
    }
    if (hasMoreThanOneComma(line))/*checks if there is more than one comma*/
    {
        printf("Error in line %d: Too many commas between numbers!\n", lineCounter);
        isError = 1;
    }
    numScanned = sscanf(line, "%s %s", word1, word2);
    if (numScanned == 1)
    {
        printf("Error in line %d: Missing operands!\n", lineCounter);
        isError = 1;
    }
    else
    {
        if (isMissingComma(line, &index) == 1)
        {
            printf("Error in line %d: Missing comma!\n", lineCounter);
            isError = 1;
        }
        else {
            findIndexAfterStr(line, &index, funcName);
            while (line[index] != ',' && line[index] != '\0')
            {
                if (arg1 != NULL && line[index] == ' ')
                {
                    break;
                }
                if (line[index] != ' ')
                {
                    arg1[i] = line[index];
                    i++;
                    arg1[i] = '\0';
                    if (line[index] == '.')
                    {
                        dotFlag = 1;
                    }
                }
                index++;
            }

            if ((strcmp(arg1, "0") == 0) || atoi(arg1) != 0) /*Meaning a number*/
            {
                if (dotFlag == 1)
                {
                    printf("Error in line %d: Expexted natural numbers!\n", lineCounter);
                    isError = 1;
                    dotFlag = 0;
                }
                type1 = 1;
            }
            else
            {
                if (arg1[0] != '\0')
                {
                    OperandValidness(arg1, &type1);
                }
            }
            if (countChar(line, ',') > 1)
            {
                printf("Error in line %d: Unnecessary text after instruction!\n", lineCounter);
                isError = 1;
            }
            else {
                i = 0;
                findIndexAfterStr(line, &index, ",");
                while (line[index] != '\0'&& line[index] != '\n')
                {
                    if (arg2 != NULL && line[index] == ' ')
                    {
                        break;
                    }
                    if (line[index] != ' ')
                    {

                        arg2[i] = line[index];
                        i++;
                        arg2[i + 1] = '\0';
                        if (line[index] == '.')
                        {
                            dotFlag = 1;
                        }
                    }
                    index++;
                }
                if (arg2[0] == '~')
                {
                    printf("Error in line %d: Missing operands!\n", lineCounter);
                    isError = 1;
                }
                else if ((strcmp(arg2, "0") == 0) || atoi(arg2) != 0) /*Meaning a number*/
                {
                    if (dotFlag == 1)
                    {
                        printf("Error in line %d: Expexted natural numbers!\n", lineCounter);
                        isError = 1;
                    }
                    type2 = 1;
                }
                else
                {
                    OperandValidness(arg2, &type2);
                }

                if (isError != 1)
                {
                    encodeFirstInfoWord(line, type1, type2, instArray, size);
                    (*L)++;
                    if (type1 == 1 && type2 == 3) /*mov -103, Length*/
                    {
                        encodeNumber(arg1, instArray, size, 0);
                        pushToInstArray("?", instArray, size);
                        (*L) += 2;
                    }
                    else if (type1 == 1 && type2 == 5) /*mov -103, @r1*/
                    {
                        encodeNumber(arg1, instArray, size, 0);
                        encodeRegister(NULL, arg2, instArray, size);
                        (*L) += 2;
                    }
                    else if (type1 == 3 && type2 == 3) /*mov Length, HELLO*/
                    {
                        pushToInstArray("?", instArray, size);
                        pushToInstArray("?", instArray, size);
                        (*L) += 2;
                    }
                    else if (type1 == 3 && type2 == 5) /*mov Length, @r1*/
                    {
                        (*L) += 2;
                        pushToInstArray("?", instArray, size);
                        encodeRegister(NULL, arg2, instArray, size);
                    }
                    else if (type1 == 5 && type2 == 3) /*mov @r1, HELLO*/
                    {
                        encodeRegister(arg1, NULL, instArray, size);
                        pushToInstArray("?", instArray, size);
                        (*L) += 2;
                    }
                    else if (type1 == 5 && type2 == 5) /*mov @r2, @r1*/
                    {
                        encodeRegister(arg1, arg2, instArray, size);
                        (*L)++;
                    }
                    else if (type1 == 1 && type2 == 1)
                    {
                        encodeNumber(arg1, instArray, size, 0);
                        encodeNumber(arg2, instArray, size, 0);
                        (*L) += 2;
                    }
                    else if (type1 == 3 && type2 == 1)
                    {
                        pushToInstArray("?", instArray, size);
                        encodeNumber(arg2, instArray, size, 0);
                        (*L) += 2;
                    }
                    else if (type1 == 5 && type2 == 1)
                    {
                        encodeRegister(arg1, NULL, instArray, size);
                        encodeNumber(arg2, instArray, size, 0);
                        (*L) += 2;
                    }
                }
            }
        }
    }
}
/*
This function deals with the 'prn' function.
*/
void prn(char* line, char*** instArray, int* size, int* L)
{
    char arg1[50];
    int i = 0;
    char word1[20];
    char word2[20];
    char funcName[20];
    int type1 = 0;
    int index = 0;
    int numScanned = 0;
    int dotFlag = 0;
    word1[19] = '\0';
    word2[19] = '\0';
    funcName[19] = '\0';
    arg1[49] = '\0';

    getFirstWord(line, funcName);
    findIndexAfterStr(line, &index, funcName);
    /* Check for errors related to commas and missing operands */
    if (line[index] == ',')
    {
        printf("Error in line %d: Illegal comma!\n", lineCounter);
        isError = 1;
    }
    numScanned = sscanf(line, "%s %s", word1, word2);
    if (numScanned == 1)
    {
        printf("Error in line %d: Missing operands!\n", lineCounter);
        isError = 1;
    }
    else
    {
        /* Extract the first operand */
        findIndexAfterStr(line, &index, funcName);
        while (line[index] != ',' && line[index] != '\0')
        {

            if (arg1 != NULL && line[index] == ' ')
            {
                break;
            }
            if (line[index] != ' ')
            {
                arg1[i] = line[index];
                i++;
                arg1[i] = '\0';
                if (line[index] == '.')
                {
                    dotFlag = 1;
                }
            }
            index++;
        }
        if ((strcmp(arg1, "0") == 0) || atoi(arg1) != 0) /*Meaning a number*/
        {
            if (dotFlag == 1)
            {
                printf("Error in line %d: Expexted natural numbers!\n", lineCounter);
                isError = 1;
            }
            type1 = 1;
        }
        else
        {
            if (arg1[0] != '\0')
                OperandValidness(arg1, &type1);
        }
        /* Check for unnecessary text after instruction */
        while (line[index] != '\0'&& line[index] != '\n')
        {
            if (line[index] != ' ')
            {
                printf("Error in line %d: Unnecessary text after instruction!\n", lineCounter);
                isError = 1;
                break;
            }
            index++;
        }
        /* Encode the instruction based on the operand type */
        if (isError != 1)
        {
            encodeFirstInfoWord(line, -1, type1, instArray, size);
            (*L)++;
            if (type1 == 3) /*not HELLO*/
            {
                pushToInstArray("?", instArray, size);
                (*L)++;
            }
            else if (type1 == 5)
            {
                encodeRegister(arg1, NULL, instArray, size);
                (*L)++;
            }
            else /* prn 1*/
            {
                encodeNumber(arg1, instArray, size, 0);
                (*L)++;
            }
        }
    }
}

/*
This function deals with the 'lea' function.
*/
void lea(char* line, char*** instArray, int* size, int* L)
{
    char arg1[50];
    char arg2[50];
    char word1[20];
    char word2[20];
    char funcName[20];
    int type1 = 0;
    int type2 = 0;
    int index = 0;
    int i = 0;
    int numScanned = 0;
    word1[19] = '\0';
    word2[19] = '\0';
    funcName[19] = '\0';
    arg2[0] = '~';
    arg1[49] = '\0';
    arg2[49] = '\0';

    getFirstWord(line, funcName);
    findIndexAfterStr(line, &index, funcName);
    /* Check for errors related to commas and missing operands */
    if (line[index] == ',')
    {
        printf("Error in line %d: Illegal comma!\n", lineCounter);
        if (countChar(line, ',') == 1)
        {
            printf("Error in line %d: Missing Parameter!\n", lineCounter);
        }
        isError = 1;
        arg1[0] = '\0';
    }
    if (hasMoreThanOneComma(line))/*checks if there is more than one comma*/
    {
        printf("Error in line %d: Too many commas between numbers!\n", lineCounter);
        isError = 1;
    }
    numScanned = sscanf(line, "%s %s", word1, word2);
    if (numScanned == 1)
    {
        printf("Error in line %d: Missing operands!\n", lineCounter);
        isError = 1;
    }
    else
    {
        if (isMissingComma(line, &index) == 1)
        {
            printf("Error in line %d: Missing comma!\n", lineCounter);
            isError = 1;
        }
        else {
            /* Extract the first operand */
            findIndexAfterStr(line, &index, funcName);
            while (line[index] != ',' && line[index] != '\0')
            {
                if (arg1 != NULL && line[index] == ' ')
                {
                    break;
                }
                if (line[index] != ' ')
                {
                    arg1[i] = line[index];
                    i++;
                    arg1[i] = '\0';
                }
                index++;
            }

            if ((strcmp(arg1, "0") == 0) || atoi(arg1) != 0) /*Meaning a number*/
            {
                printf("Error in line %d: Wrong type of operand (expected label)!\n", lineCounter);
                isError = 1;
            }
            else
            {
                if (arg1[0] != '\0')
                {
                    OperandValidness(arg1, &type1);
                }
                if (type1 != 3)
                {
                    printf("Error in line %d: Wrong type of operand (expected label)!\n", lineCounter);
                    isError = 1;
                }
            }
            /* Continue processing the second operand */
            if (countChar(line, ',') > 1)
            {
                printf("Error in line %d: Unnecessary text after instruction!\n", lineCounter);
                isError = 1;
            }
            else {
                index++;
                while (line[index] == ' ')
                {
                    index++;
                }
                if (line[index] == '\0' || line[index] == '\n')
                {
                    printf("Error in line %d: Missing operands!\n", lineCounter);
                    isError = 1;
                }
                else
                {
                    i = 0;
                    findIndexAfterStr(line, &index, ",");
                    while (line[index] != '\0'&& line[index] != '\n')
                    {
                        if (arg2 != NULL && line[index] == ' ')
                        {
                            break;
                        }
                        if (line[index] != ' ')
                        {

                            arg2[i] = line[index];
                            i++;
                            arg2[i + 1] = '\0';
                        }
                        index++;
                    }
                    if (arg2[0] == '~')
                    {
                        printf("Error in line %d: Missing operands!\n", lineCounter);
                        isError = 1;
                    }
                    else if ((strcmp(arg2, "0") == 0) || atoi(arg2) != 0)
                    {
                        printf("Error in line %d: Operand shouldn't be a number!\n", lineCounter);
                        isError = 1;
                    }
                    else {
                        OperandValidness(arg2, &type2);
                    }

                    if (isError != 1)
                    {
                        encodeFirstInfoWord(line, type1, type2, instArray, size);
                        (*L)++;
                        if (type1 == 3 && type2 == 3)
                        {
                            pushToInstArray("?", instArray, size);
                            pushToInstArray("?", instArray, size);
                            (*L) = (*L) + 2;
                        }
                        else if (type1 == 3 && type2 == 5)
                        {
                            (*L) = (*L) + 2;
                            pushToInstArray("?", instArray, size);
                            encodeRegister(NULL, arg2, instArray, size);
                        }
                    }
                }
            }
        }
    }
}

/* This function addes IC to all the data address*/

void updateSymbolTable(List* symbolTable, int IC)
{
    Node* current = symbolTable->head;
    while (current != NULL) {
        if (strcmp(current->type, "relocatable") == 0)
        {
            current->sourceAddress = current->sourceAddress + IC;
        }
        current = current->next;
    }
}

/*This function adds the entry label to the symbolTable*/

void addEntryLabels(char* line, List* symbolTable)
{
    int index = 0;
    int newLabelIndex = 0;
    int value = 0;
    char str[80];
    int len;
    char* newLabel = (char*)malloc(32 * sizeof(char));
    Node* current=NULL;
    getFirstWord(line, str);
    findIndexAfterStr(line, &index, str);
    len = (int)strlen(line);
    if (index >= len) /* checks if there is no label */
    {
        printf("Error in line %d: No labels after \".extern\"!\n", lineCounter);
        isError = 1;
    }
    if (line[len - 1] == ',') /* last char is comma*/
    {
        printf("Error in line %d: No label after comma!\n", lineCounter);
        isError = 1;
    }
    if (line[index] == ',') /* no labl before comma */
    {
        printf("Error in line %d: No label after comma!\n", lineCounter);
        isError = 1;
    }
    if (hasMoreThanOneComma(line))/*checks if there is more than one comma*/
    {
        printf("Error in line %d: Too many commas between numbers!\n", lineCounter);
        isError = 1;
    }
    if (isMissingComma(line, &index) == 1)/*checks if there is missing comma between labels*/
    {
        printf("Error in line %d: Missing comma!\n", lineCounter);
        isError = 1;
    }
    if (isError == 1)
    {
        return;
    }
    else
    {
        /* Loop through the line to extract entry labels */
        while (line[index] != '\0' && line[index] != '\n')
        {
            newLabel = (char*)calloc(sizeof(newLabel), sizeof(char));
            /* Extract the label from the line */
            while (line[index] != ',' && line[index] != '\0' && line[index] != '\n')
            {
                if (line[index] != ' ')
                {
                    newLabel[newLabelIndex] = line[index];
                    newLabelIndex++;
                }
                index++;
            }
            /* Process the extracted entry label */
            if (newLabel)
            {
                newLabel[newLabelIndex] = '\0';
                if (exist(newLabel, symbolTable) == 1)
                {
                    /* change relocatable to entry*/
                    current = symbolTable->head;
                    while (current != NULL) {
                        if (strcmp(current->name, newLabel) == 0)
                        {
                            strncpy(current->type, "entry", 24);
                            break;
                        }
                        current = current->next;
                    }
                }
                else
                {
                    /* Check label validity and add to symbol table */
                    if (strcmp(newLabel, "")!=0) {
                    labelValidness(newLabel);
                    addToSymbolTable(newLabel, symbolTable, &value, "entry");
                    }
                }

                index++;
                newLabelIndex = 0;
                free(newLabel);
            }
        }
    }
}

/*This function checks the type of the operand and updates the num*/


void checkType(char* arg, int* num)
{
    if (arg[0] == '@')
    {
        *num = 5;
    }
    else if ((strcmp(arg, "0") == 0) || atoi(arg) != 0)
    {
        *num = 1;
    }
    else {
        *num = 3;
    }
}

/*This function updates the address of the externals type*/

void getSource(int* num, char* labelName, List* symbolTable, int* isExtern)
{
    Node* tempNode = symbolTable->head;
    while (tempNode != NULL)
    {
        if (strcmp(tempNode->name, labelName) == 0)
        {
            if (strcmp(tempNode->type, "external") == 0)
            {
                *isExtern = 1;
            }
            /* Set the source address */
            *num = tempNode->sourceAddress;
            /* Increment source address for non-external labels */
            if (strcmp(tempNode->type, "external") != 0)
            {
                *num = *num + 100;
            }
            break;
        }
        tempNode = tempNode->next;
    }
}

/*This function is used in the second pass, searches for label in group 1 and translate to bin the correct ones in the array*/

void updateLabelG1(char* line, List* symbolTable, char*** instArray, int* IC, externList* extList)
{
    int isExtern = 0;
    int index = 0;
    int type1 = 0;
    int type2 = 0;
    char funcName[80];
    char arg1[80];
    char arg2[80];
    char sourceStr[80];
    int i = 0;
    int source = 0;
    Node* temp = (Node*)malloc(sizeof(Node));
    getFirstWord(line, funcName);
    findIndexAfterStr(line, &index, funcName);
    /*Parse the first argument*/
    while (line[index] != ',' && line[index] != '\0' && line[index] != '\n')
    {
        if (arg1 != NULL && line[index] == ' ')
        {
            break;
        }
        if (line[index] != ' ')
        {
            arg1[i] = line[index];
            i++;
            arg1[i] = '\0';
        }
        index++;
    }
    /*Reset index and i for the second argument*/
    i = 0;
    findIndexAfterStr(line, &index, ",");
    while (line[index] != '\0' && line[index] != '\n')
    {
        if (arg2 != NULL && line[index] == ' ')
        {
            break;
        }
        if (line[index] != ' ')
        {

            arg2[i] = line[index];
            i++;
            arg2[i + 1] = '\0';
        }
        index++;
    }
    /* check the types of args -> L*/
    checkType(arg1, &type1);
    checkType(arg2, &type2);
    (*IC)++;
    /*Increment the instruction counter*/
    if (type1 == 1 && type2 == 3) /*mov -103, Length*/
    {
        (*IC) += 2;
        /*search for label in symboltable*/
        if (exist(arg2, symbolTable) == 0)
        {
            printf("Error in line %d: Unknown label name!\n", lineCounter);
            isError = 1;
        }
        else
        {
            getSource(&source, arg2, symbolTable, &isExtern);
            sprintf(sourceStr, "%d", source);
            if (isExtern == 1)
            {
                encodeNumber(sourceStr, instArray, IC, 2);
                isExtern = 0;
            }
            else
            {
                encodeNumber(sourceStr, instArray, IC, 1);
            }
        }
    }
    else if (type1 == 1 && type2 == 5) /*mov -103, @r1*/
    {
        (*IC) += 2;
    }
    else if (type1 == 3 && type2 == 3) /*mov Length, HELLO*/
    {
        (*IC)++;
        /*search for label in symboltable*/
        if (exist(arg1, symbolTable) == 0)
        {
            printf("Error in line %d: Unknown label name!\n", lineCounter);
            isError = 1;
        }
        else
        {
            getSource(&source, arg1, symbolTable, &isExtern);
            sprintf(sourceStr, "%d", source);
            if (isExtern == 1)
            {
                encodeNumber(sourceStr, instArray, IC, 2);
                isExtern = 0;
            }
            else
            {
                encodeNumber(sourceStr, instArray, IC, 1);
            }
        }
        (*IC)++;
        /*search for label in symboltable*/
        if (exist(arg2, symbolTable) == 0)
        {
            printf("Error in line %d: Unknown label name!\n", lineCounter);
            isError = 1;
        }
        else
        {
            getSource(&source, arg2, symbolTable, &isExtern);
            sprintf(sourceStr, "%d", source);
            if (isExtern == 1)
            {
                encodeNumber(sourceStr, instArray, IC, 2);
                isExtern = 0;
            }
            else
            {
                encodeNumber(sourceStr, instArray, IC, 1);
            }
        }
    }
    else if (type1 == 3 && type2 == 5) /*mov Length, @r1*/
    {
        (*IC)++;
        /*search for label in symboltable*/
        if (exist(arg1, symbolTable) == 0)
        {
            printf("Error in line %d: Unknown label name!\n", lineCounter);
            isError = 1;
        }
        else
        {
            getSource(&source, arg1, symbolTable, &isExtern);
            sprintf(sourceStr, "%d", source);
            if (isExtern == 1)
            {
                encodeNumber(sourceStr, instArray, IC, 2);
                isExtern = 0;
            }
            else
            {
                encodeNumber(sourceStr, instArray, IC, 1);
            }
        }
        (*IC)++;
    }
    else if (type1 == 5 && type2 == 3) /*mov @r1, HELLO*/
    {
        (*IC)++;
        if (exist(arg2, symbolTable) == 0)
        {
            printf("Error in line %d: Unknown label name!\n", lineCounter);
            isError = 1;
        }
        else
        {
            getSource(&source, arg2, symbolTable, &isExtern);
            sprintf(sourceStr, "%d", source);
            if (isExtern == 1)
            {
                encodeNumber(sourceStr, instArray, IC, 2);
                isExtern = 0;
            }
            else
            {
                encodeNumber(sourceStr, instArray, IC, 1);
            }
        }
        (*IC)++;
    }
    else if (type1 == 5 && type2 == 5) /*mov @r2, @r1*/
    {
        (*IC) += 1;
    }
    else if (type1 == 1 && type2 == 1)
    {
        (*IC) += 2;
    }
    else if (type1 == 3 && type2 == 1)
    {
        (*IC)++;
        if (exist(arg1, symbolTable) == 0)
        {
            printf("Error in line %d: Unknown label name!\n", lineCounter);
            isError = 1;
        }
        else
        {
            getSource(&source, arg1, symbolTable, &isExtern);
            sprintf(sourceStr, "%d", source);
            if (isExtern == 1)
            {
                encodeNumber(sourceStr, instArray, IC, 2);
                isExtern = 0;
            }
            else
            {
                encodeNumber(sourceStr, instArray, IC, 1);
            }
        }
        (*IC)++;
    }
    else if (type1 == 5 && type2 == 1)
    {
        (*IC) += 2;
    }
    /*Handle external labels and update extList*/
    temp = symbolTable->head;
    while (temp != NULL)
    {
        if (strcmp(temp->type, "external") == 0)
        {
            if (strcmp(arg1, temp->name) == 0)
            {
                addToExtEnd(arg1, IC, extList);
            }
            if (strcmp(arg2, temp->name) == 0)
            {
                addToExtEnd(arg2, IC, extList);
            }
        }
        temp = temp->next;
    }
}

/*This function is used in the second pass, searches for label in group 2 and translate to bin the correct ones in the array*/

void updateLabelG2(char* line, List* symbolTable, char*** instArray, int* IC, externList* extList)
{
    int isExtern = 0;
    int index = 0;
    int type1 = 0;
    char funcName[80];
    char arg1[80];
    char sourceStr[80];
    int i = 0;
    int source = 0;
    Node* temp = (Node*)malloc(sizeof(Node));
    getFirstWord(line, funcName);
    findIndexAfterStr(line, &index, funcName);
    /*Parse the first argument*/
    while (line[index] != ',' && line[index] != '\0' && line[index] != '\n')
    {
        if (arg1 != NULL && line[index] == ' ')
        {
            break;
        }
        if (line[index] != ' ')
        {
            arg1[i] = line[index];
            i++;
            arg1[i] = '\0';
        }
        index++;
    }
    /* check the types of args -> L*/
    checkType(arg1, &type1);
    (*IC)++;
    if (type1 == 1)
    {
        (*IC)++;
    }
    if (type1 == 3)
    {
        if (exist(arg1, symbolTable) == 0)
        {
            printf("Error in line %d: Unknown label name!\n", lineCounter);
            isError = 1;
        }
        else
        {
            getSource(&source, arg1, symbolTable, &isExtern);
            sprintf(sourceStr, "%d", source);
            /* Encode the source based on whether it's external or not*/
            if (isExtern == 1)
            {
                encodeNumber(sourceStr, instArray, IC, 2);
                isExtern = 0;
            }
            else
            {
                encodeNumber(sourceStr, instArray, IC, 1);
            }
        }
        (*IC)++;
        temp = symbolTable->head;
        while (temp != NULL)
        {
            if (strcmp(temp->type, "external") == 0)
            {
                if (strcmp(arg1, temp->name) == 0)
                {
                    addToExtEnd(arg1, IC, extList);
                    break;
                }
            }
            temp = temp->next;
        }
    }
    if (type1 == 5)
    {
        (*IC)++;
    }

}
/*This function send every instruction to his specific update label function */
void encodeLabel(char* line, List* symbolTable, char*** instArray, int* IC, externList* extList)
{
    char str[80];
    getFirstWord(line, str);
    if (strcmp(str, "mov") == 0) {
        updateLabelG1(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "cmp") == 0) {
        updateLabelG1(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "add") == 0) {
        updateLabelG1(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "sub") == 0) {
        updateLabelG1(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "not") == 0) {
        updateLabelG2(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "clr") == 0) {
        updateLabelG2(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "lea") == 0) {
        updateLabelG1(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "inc") == 0) {
        updateLabelG2(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "dec") == 0) {
        updateLabelG2(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "jmp") == 0) {
        updateLabelG2(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "bne") == 0) {
        updateLabelG2(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "red") == 0) {
        updateLabelG2(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "prn") == 0) {
        updateLabelG2(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "jsr") == 0) {
        updateLabelG2(line, symbolTable, instArray, IC, extList);
    }
    else if (strcmp(str, "rts") == 0) {
        (*IC)++;
    }
    else if (strcmp(str, "stop") == 0) {
        (*IC)++;
    }
}

/*This function translate numbers to a string in base 64*/
void number_to_base64(unsigned int num, char* base64, int num_digits) {
    int i;
    for (i = num_digits - 1; i >= 0; i--) {
        base64[i] = base64_table[num & 0x3F];
        num >>= 6;
    }
    base64[num_digits] = '\0';
}

/*This function translate binary code to decimal*/

int binary_to_decimal(const char* binary) {
    int i;
    int decimal = 0;
    int length = (int)strlen(binary);

    for (i = 0; i < length; i++) {
        decimal = decimal * 2 + (binary[i] - '0');
    }

    return decimal;
}

/*frees the memory of the symbolTable and the nodes inside the table*/
void freeList(List* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

/*frees the memory of the extern list and the externNodes inside the table*/

void freeExtList(externList* list) {
    externNode* current = list->head;
    externNode* temp = NULL;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    /* Reset the list's attributes */
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

/*
Frees the memory occupied by arrays of macro names and macro content.
*/

void free_arraysAssembler(char*** macroNames, char*** macroContent)
{
    int i = 0;
    /* Free the macro names array*/
    if (*macroNames != NULL) {
        if (strcmp((*macroNames)[0], "start") != 0)
        {
            for (i = 0; (*macroNames)[i] != NULL; i++) {
                free((*macroNames)[i]);  /* Free each individual string*/
            }
        }
        *macroNames = NULL;  /* Set the pointer to NULL*/
    }

    /* Free the macro content array*/
    if (*macroContent != NULL) {
        if (strcmp((*macroContent)[0], "start") != 0)
        {
            for (i = 0; (*macroContent)[i] != NULL; i++) {
                free((*macroContent)[i]);  /* Free each individual string*/
            }
        }
        *macroContent = NULL;  /* Set the pointer to NULL*/
    }
}

/*
This function responsible for the assembler process (transitions 1 & 2)
The function gets:
1) A file name
2) Pointer to a struct named "List" which we'll use as our symbolTable
*/
void Assembler(char* fileName, List* symbolTable)
{
    /* Declarations for various variables used in the function*/
    char amPath[100];
    FILE* filePointer = NULL;
    int* DC = (int*)malloc(sizeof(int));
    int* IC = (int*)malloc(sizeof(int));
    int* L = (int*)malloc(sizeof(int));
    int size = 10;
    int isExtern = 0;
    int isEntry = 0;
    char line[80];
    char* labelName = (char*)malloc(80 * sizeof(char));
    char* firstWord = (char*)malloc(80 * sizeof(char));
    int* typeHolder = (int*)malloc(sizeof(int));
    Node* tempNode = (Node*)malloc(sizeof(Node)); /* Allocate memory for a new Node*/
    externNode* tempExternalNode = (externNode*)malloc(sizeof(Node)); /* Allocate memory for a new Node*/
    char** instArray = (char**)malloc(10 * sizeof(char*));
    char** dataArray = (char**)malloc(10 * sizeof(char*));
    externList extList = { 0,NULL,NULL };
    int i = 0;
    int number = 0;
    char numberStr[10];
    char obPath[80];
    FILE* obFile = NULL;
    char tempName[80];
    int tempAddress = 0;
    char entPath[80];
    char extPath[80];
    FILE* entFile = NULL;
    FILE* extFile = NULL;
    isError = 0;
    lineCounter = 1;

    strcpy(amPath, "./");
    strcat(amPath, fileName);
    strcat(amPath, ".am");
    
    /*getting a pointer to the file*/
    filePointer = fopen(amPath, "r");

    if (!filePointer) {
        printf("Error opening the file in the path %s\n", amPath);
        exit(1);
    }


    /*stage 1 - אתחל DC->0, IC->0*/
    *DC = 0;
    *IC = 0;
    *L = 0;

    *instArray = "start";
    *dataArray = "start";

    if (dataArray == NULL || instArray == NULL || tempNode == NULL || typeHolder == NULL || firstWord == NULL || labelName == NULL || L == NULL || IC == NULL || DC == NULL)
    {
        printf("Memory allocation failed\n");
        exit(0);
    }


    size = 0;


    /*stage 2 - קרא את השורה הבאה מקובץ המקור. אם נגמר קובץ המקור, עבור ל-16*/
    while (getLine(filePointer, line, sizeof(line)) != 0)/*Checks if we reached the end of the file*/
    {
        /*stage 3 - האם השדה הראשון הוא סמל? אם לא, עבור ל-5*/
        if (isLabel(line) == 0)/*Checks if there is label in the line*/
        {
            /*stage 4 הדלק דגל "יש הגדרת סמל"*/
            labelFlag = 1;/*The flag now shows that there is label definition*/
            getFirstWord(line, labelName);/*Gets the label name*/
            removeLastChar(labelName);/*Removes the colon from the label name and update the label name*/
        }
        /*stage 5 - האם זוהי הנחיה לאחסון נתונים, כלומר, האם הנחית data. או string.? אם לא, עבור ל-8. */
        isDataStroage(line, typeHolder);
        if (*typeHolder == 1 || *typeHolder == 0)
        {
            /*stage 6*/
            if (labelFlag == 1)
            {
                addToSymbolTable(labelName, symbolTable, DC, "relocatable");
                labelFlag = 0;
            }
            /*stage 7*/
            if (*typeHolder == 0)
            {
                encodeStringData(line, &dataArray, DC);
            }
            else
            {
                encodeData(line, &dataArray, DC);
            }
        }
        else
        {
            /*stage 8*/
            isExternOrEntry(line, typeHolder);
            if (*typeHolder == 1)
            {
                isExtern = 1;
                /*stage 9*/
                addExternLabels(line, symbolTable, &extList);
            }
        }
        /*stage 11*/
        if (labelFlag == 1)
        {
            addToSymbolTable(labelName, symbolTable, IC, "code"); /*check what is the code sign*/
        }
        /*stage 12 + 13*/
        if (*typeHolder == -1)
        {
            encodeInstruction(line, &instArray, &size, L);
        }
        /*stage 14*/
        (*IC) = (*IC) + (*L);
        (*L) = 0;
        labelFlag = 0;
        lineCounter++;
    }

    /*stage 16*/
    if (isError != 1)
    {
        /*stage 17*/
        updateSymbolTable(symbolTable, (*IC));

        fclose(filePointer);

        filePointer = fopen(amPath, "r");

        if (!filePointer) {
            printf("Error opening the file in the path %s\n", amPath);
            exit(1);
        }
        /* second pass*/
        /*stage 1*/
        *IC = 0;
        lineCounter = 1;
        /*stage 2*/
        while (getLine(filePointer, line, sizeof(line)) != 0)
        {
            /*stage 3*/
            if (isLabel(line) == 0)/*Checks if there is label in the line*/
            {
                removeFirstWord(line);
            }
            /*stage 4*/
            isDataStroage(line, typeHolder);
            if (*typeHolder != 0 && *typeHolder != 1)
            {
                isExternOrEntry(line, typeHolder);
                /*stage 5*/
                if (*typeHolder == 0) /*Meaning ".entry"*/
                {
                    isEntry = 1;
                    /*stage 6*/
                    addEntryLabels(line, symbolTable);
                }
                /*stage 7*/
                encodeLabel(line, symbolTable, &instArray, IC, &extList);
            }
            lineCounter++;
        }

        tempNode = symbolTable->head;
        while (tempNode != NULL)
        {
            if (strcmp(tempNode->type, "entry") == 0 && tempNode->sourceAddress == 0)
            {
                printf("Error: Undefined entry label: %s!\n", tempNode->name);
                isError = 1;
            }
            tempNode = tempNode->next;
        }

        /*stage 10*/
        if (isError != 1)
        {
            /*stage 11*/

            /* .ob*/
            strcpy(obPath, "./");
            strcat(obPath, fileName);
            strcat(obPath, ".ob");

            obFile = fopen(obPath, "w");

            fprintf(obFile, "%d %d\n", *IC, *DC);
            /* "000000000000"*/
            if (strcmp(*instArray, "start"))
            {
                while (instArray[i] != NULL)
                {
                    number = binary_to_decimal(instArray[i]);
                    number_to_base64(number, numberStr, 2);
                    fprintf(obFile, "%s\n", numberStr);
                    i++;
                }
            }
            i = 0;
            if (dataArray != NULL && strcmp(dataArray[0], "start") != 0)
            {
                while (dataArray[i] != NULL)
                {
                    number = binary_to_decimal(dataArray[i]);
                    number_to_base64(number, numberStr, 2);
                    if (dataArray[i + 1] != NULL)
                    {
                        fprintf(obFile, "%s\n", numberStr);
                    }
                    else
                    {
                        fprintf(obFile, "%s", numberStr);
                    }
                    i++;
                }
            }

            fclose(obFile);

            /* .ent*/
            if (isEntry == 1)
            {
                strcpy(entPath, "./");
                strcat(entPath, fileName);
                strcat(entPath, ".ent");
                tempName[0] = '\0';
                entFile = fopen(entPath, "w");
                tempNode = symbolTable->head;
                while (tempNode != NULL)
                {
                    if (strcmp(tempNode->type, "entry") == 0)
                    {
                        if (tempName[0] == '\0')
                        {
                            strcpy(tempName, tempNode->name);
                            tempAddress = tempNode->sourceAddress + 100;
                        }
                        else
                        {
                            fprintf(entFile, "%s %d\n", tempName, tempAddress);
                            strcpy(tempName, tempNode->name);
                            tempAddress = tempNode->sourceAddress + 100;
                        }
                    }
                    tempNode = tempNode->next;
                }
                fprintf(entFile, "%s %d", tempName, tempAddress);
                fclose(entFile);
            }
            /* .ext*/
            if (isExtern == 1)
            {
                strcpy(extPath, "./");
                strcat(extPath, fileName);
                strcat(extPath, ".ext");
                tempExternalNode = (&extList)->head;
                if (tempExternalNode != NULL)
                {
                    extFile = fopen(extPath, "w");
                }
                while (tempExternalNode != NULL)
                {
                    if (tempExternalNode->next != NULL)
                    {
                        fprintf(extFile, "%s %d\n", tempExternalNode->name, (tempExternalNode->sourceAddress + 100));
                    }
                    else
                    {
                        fprintf(extFile, "%s %d", tempExternalNode->name, (tempExternalNode->sourceAddress + 100));
                    }
                    tempExternalNode = tempExternalNode->next;
                }
                tempExternalNode = (&extList)->head;
                if (tempExternalNode != NULL)
                {
                    fclose(extFile);
                }

            }
            /* free all memory used!*/
            free_arraysAssembler(&instArray, &dataArray);
            freeList(symbolTable);
            freeExtList(&extList);
        }
    }
}
