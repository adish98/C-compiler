#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "preAssembler.h"

int isMacro = 1; /*This variable indicates wheter we are in a macro (0) or not (1)*/

/*
This function will check the line is blank.
return 1- if true else 0.
*/
int isLineBlank(const char* line)
{
    while (*line) {
        if (!isspace((unsigned char)*line)) {
            return 0;  /* Non - whitespace character found, line is not blank */
        }
        line++;
    }

    return 1;  /* Line contains only whitespace characters*/
}
/*
This function will check if the line is a commant line.
input One line from the input file.
return 1- If this line is a commant, if not return 0.
*/
int isCommant(const char* line)
{
    char ch;
    sscanf(line, " %c", &ch);
    if (ch == ';') {
        return 1;
    }
    return 0;
}

/*
This function will read a line from the file.
Then will check if not NULL
*/
int getLine(FILE* filePointer, char* line, int size)
{
    if (fgets(line, 79, filePointer) == NULL) {
        /* If fgets returns NULL, it means the end of file is reached or an error occurred*/
        line[0] = '\0';  /* Set the line as an empty string to indicate the end of file*/
        return 0;
    }
    return 1;
}

/*
This funciton checks if we reacehd the end of the macro
retrun 1- If we reached the end of macro, else return 0
*/
int isEndMacro(const char* line)
{
    char firstWord[50];
    if (sscanf(line, " %49s", firstWord) == 1) {
        if (strcmp("endmcro", firstWord) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
This function checks if the first word in the line is "mcro"
return 1 if true, else return 0
*/
int isStartMacro(const char* line) {
    char firstWord[50];
    if (sscanf(line, " %49s", firstWord) == 1) {
        if (strcmp("mcro", firstWord) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
This function gets an array and adds an empty cell at the end of the array.
*/
void increaseMcroConByOne(char*** array) {
    int i = 0;
    int currentSize = 0;
    char** currentArray = NULL;
    char** newArray = NULL;
    int newSize = 0;

    /* Check if the array is empty (NULL) */
    if (*array == NULL) {
        /* Allocate memory for a new array with two elements: one for the new cell and one for NULL termination*/
        newArray = (char**)malloc(2 * sizeof(char*));

        /* Check if memory allocation failed*/
        if (newArray == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }

        /* Allocate memory for an empty string for the new cell*/
        newArray[0] = (char*)malloc(1 * sizeof(char));
        newArray[0][0] = '\0'; /* Assign an empty string*/

        /* Add a NULL element at the end of the new array*/
        newArray[1] = NULL;

        /* Update the array pointer to point to the new array*/
        *array = newArray;

        return;
    }

    /* Determine the current size of the array*/
    currentArray = *array;
    while (currentArray[currentSize] != NULL) {
        currentSize++;
    }

    /* Allocate memory for a new array with increased size*/
    newSize = currentSize + 2; /* Increase size by one and add an extra NULL element*/
    newArray = (char**)malloc(newSize * sizeof(char*));

    /* Check if memory allocation failed*/
    if (newArray == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    /* Copy the contents of the old array into the new array*/
    for (i = 0; i < currentSize; i++) {
        newArray[i] = currentArray[i];
    }

    /* Allocate memory for an empty string for the new cell*/
    newArray[currentSize] = (char*)malloc(1 * sizeof(char));
    newArray[currentSize][0] = '\0'; /* Assign an empty string*/

    /* Add a NULL element at the end of the new array*/
    newArray[currentSize + 1] = NULL;

    /* Deallocate memory for the old array*/
    free(currentArray);

    /* Update the array pointer to point to the new array*/
    *array = newArray;
}




/*
This function gets the macro names array and a macro line and inserts the macro name into the array.
*/
void insertMacroName(char*** macroNames, char* line)
{
    char macroName[76];
    int numElements = 0;
    sscanf(line, "mcro %s", macroName);

    /* Find the number of elements in the macroNames array*/
    if (*macroNames != NULL)
    {
        while ((*macroNames)[numElements] != NULL)
            numElements++;
    }

    /* Reallocate memory for the macroNames array with space for the new macro name*/
    *macroNames = realloc(*macroNames, (numElements + 2) * sizeof(char*));
    if (*macroNames == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }


    /* Allocate memory for the new macro name*/
    (*macroNames)[numElements] = malloc((strlen(macroName) + 1) * sizeof(char));
    if ((*macroNames)[numElements] == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    /* Copy the macro name into the macroNames array*/
    strcpy((*macroNames)[numElements], macroName);
    (*macroNames)[numElements + 1] = NULL; /* Mark the end of the array*/

}

/*
This function checks if the line is contain an existing macro name.
return index- if an existing macro name was found else return -1.
*/
int isExistingMacro(char*** macroNames, const char* line)
{
    int i = 0;
    char first[25];
    char second[25];
    int num = sscanf(line, "%24s %24s", first, second);
    if (*macroNames == NULL) {
        return -1; /* No macro names exist, return 0*/
    }
    if (num == 1) {
        /* Check if macroName exists in the macroNames array*/
        for (i = 0; (*macroNames)[i] != NULL; i++) {
            if (strcmp((*macroNames)[i], first) == 0) {
                return i; /* Macro name found, return 1*/
            }
        }
    }
    else {
        /* Check if macroName exists in the macroNames array*/
        for (i = 0; (*macroNames)[i] != NULL; i++) {
            if (strcmp((*macroNames)[i], second) == 0) {
                return i; /* Macro name found, return 1*/
            }
        }
    }

    return -1; /* Macro name not found, return 0*/
}

/*
This function gets a filePointer and a line and inserts the line to the file.
*/
void insertLineToFile(FILE* filePointer, char* line)
{
    fprintf(filePointer, "%s", line);
}

/*
This function gets a file pointer and a macro content and inserts the content to the file.
*/
void insertMacroToFile(FILE* filePointer, char* content)
{
    fprintf(filePointer, "%s", content);
}

/*
This function get the macro content array and a line.
The function would add the line to the array.
*/
void addLineToArray(char*** macroContent, char* line) {
    /* Determine the current size of the 'macroContent' array*/
    int currentSize = 0;
    char** currentArray = *macroContent;
    int lineLength = (int)strlen(line);
    char* lastLine = NULL;
    int lastLineLength = 0;
    char* newLastLine = NULL;
    while (currentArray[currentSize] != NULL) {
        currentSize++;
    }

    /* Reallocate memory for the last cell to accommodate the new line*/
    lineLength = (int)strlen(line);
    lastLine = currentArray[currentSize - 1];
    lastLineLength = (int)strlen(lastLine);
    newLastLine = realloc(lastLine, (lastLineLength + lineLength + 2) * sizeof(char));
    if (newLastLine == NULL) {
        /* Handle memory allocation error*/
        printf("Error: Failed to allocate memory for the last line.\n");
        return;
    }

    /* Concatenate the new line to the last line*/
    strcat(newLastLine, line);
    /*strcat(newLastLine, "\n");*/

    /* Update the last cell in the 'macroContent' array*/
    currentArray[currentSize - 1] = newLastLine;
}

/*
This function adds the contant of the macro to the array at the given index
return the content if exist if not return NULL.
*/
char* getMacroContent(char** macroContent, int index)
{
    int i = 0;
    if (macroContent == NULL) {
        return NULL; /* Array is empty*/
    }


    while (macroContent[i] != NULL) {
        if (i == index) {
            return macroContent[i];
        }
        i++;
    }

    return NULL; /* Index is out of bounds*/
}


/*
This function frees that 'macroName' and 'macroContent'.
*/
void free_arrays(char*** macroNames, char*** macroContent)
{
    int i = 0;
    /* Free the macro names array*/
    if (*macroNames != NULL) {
        for (i = 0; (*macroNames)[i] != NULL; i++) {
            free((*macroNames)[i]);  /* Free each individual string*/
        }
        /*free(*macroNames);  Free the array itself*/
        *macroNames = NULL;  /* Set the pointer to NULL*/
    }

    /* Free the macro content array*/
    if (*macroContent != NULL) {
        for (i = 0; (*macroContent)[i] != NULL; i++) {
            free((*macroContent)[i]);  /* Free each individual string*/
        }
        /*free(*macroContent);  Free the array itself*/
        *macroContent = NULL;  /* Set the pointer to NULL*/
    }
}


int preAssembler(FILE* filePointer, char* fileName)
{
    char amPath[100];
    char line[80];
    FILE* newFile = NULL;
    char** macroNames = NULL; /*An array that holds the macro name*/
    char** macroContent = NULL; /*An array that holds the macro content*/
    int num;
    int emptyFileFlag = 0;/*will remain zero if the file is empty*/

    strcpy(amPath, "./");
    strcat(amPath, fileName);
    strcat(amPath, ".am");

    newFile = fopen(amPath, "w");

    if (!newFile) {
        printf("Error opening the file in the path %s\n", amPath);
        exit(1);
    }

    while (getLine(filePointer, line, sizeof(line)) != 0)
    {
        if (isLineBlank(line) == 0 && isCommant(line) == 0) {
            emptyFileFlag++;
            if (isMacro == 0) {
                if (isEndMacro(line) == 1) {
                    isMacro = 1;
                }
                else {
                    addLineToArray(&macroContent, line);
                }
            }
            else if (isStartMacro(line) == 1) { /*New Mcro definition*/
                isMacro = 0;
                insertMacroName(&macroNames, line);
                increaseMcroConByOne(&macroContent);
            }
            else if ((num = isExistingMacro(&macroNames, line)) != -1) {
                char* mcroText = getMacroContent(macroContent, num);
                insertMacroToFile(newFile, mcroText);
            }
            else { /* just regular line */
                insertLineToFile(newFile, line);
            }
        }
    }

    free_arrays(&macroNames, &macroContent);
    fclose(newFile);
    if (emptyFileFlag == 0)
    {
        remove(amPath);
        return 1;
    }
    return 0;
}

