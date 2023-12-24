assembler: main.o node.o preAssembler.o Assembler.o
	gcc -ansi -Wall -g main.o node.o preAssembler.o Assembler.o -o assembler -lm

main.o: main.c node.h Assembler.h preAssembler.h
	gcc -c -g -Wall -ansi -pedantic main.c -o main.o

node.o: node.c node.h
	gcc -c -g -Wall -ansi -pedantic node.c -o node.o

preAssembler.o: preAssembler.c preAssembler.h
	gcc -c -g -Wall -ansi -pedantic preAssembler.c -o preAssembler.o

Assembler.o: Assembler.c Assembler.h preAssembler.h node.h
	gcc -c -g -Wall -ansi -pedantic Assembler.c -o Assembler.o
