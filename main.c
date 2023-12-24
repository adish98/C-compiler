#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preAssembler.h"
#include "Assembler.h"
#include "node.h"

int main(int argc, char** argv) {
 
    char fileName[100];
    char path[100];
    List symbolTable = {0, NULL, NULL};/*builds the symbolTable*/
    FILE* filePointer = NULL;
    int i = 1;
	
    if (argc < 2)
    {
        printf("Missing file name!\n");
        return 1; 
    }

    for(i=1; i<argc; i++)
    {
        strcpy(fileName, argv[i]);
        printf("------ Processing file: %s ------\n",fileName);
        strcpy(path, "./");
        strcat(path, fileName);
        strcat(path, ".as");

        filePointer = fopen(path, "r");

        if (!filePointer) {
            printf("********Error opening the file in the path %s********\n\n", path);
            continue;
        }

        /* pre assembler */
        if (preAssembler(filePointer, fileName) == 1)
        {
            printf("------ File %s is empty ------\n\n", fileName);
            continue;
        }
        fclose(filePointer);
        /* actual assembler */
        Assembler(fileName, &symbolTable);
        symbolTable.head = NULL;
        symbolTable.tail = NULL;
        symbolTable.count = 0;
  printf("------ Processing finished ------\n\n");
    }
    return 0;
}
