I built a compiler that will read an Assembly language code and convert it to Binary code,
This compiler has a three stages of the compile processes:
The first step (Pre-Assembler) is reading the assembly code and converting it to full length code (without macros)
Second step will be the first pass on the assembly code and will make all the characterization of this input (symbols, type of order) and translating every command to binary code
The third and last step will be a continuation of the second pass, translating the rest of the command and sending the data into output documents.
In the end the program will export four documents (.am, .ob, .ent, .ext)
*This program is suitable to run in Linux operating system.

To start the compailer please run Main.c (type in cmd after opening the right folder: make and than insert ./assembler input1 input2 input3)
