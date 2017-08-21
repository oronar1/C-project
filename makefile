maman14: cmdProcess.o fileParsing.o outputfiles.o wordParsing.o main.o
	gcc -Wall -ansi -pedantic cmdProcess.o fileParsing.o outputfiles.o wordParsing.o main.o -o assembler
 
cmdProcess.o: cmdProcess.c global.h project.h
	gcc -Wall -ansi -pedantic cmdProcess.c -c

fileParsing.o: fileParsing.c global.h project.h
	gcc -Wall -ansi -pedantic fileParsing.c -c 

outputfiles.o: outputfiles.c global.h project.h
	gcc -Wall -ansi -pedantic outputfiles.c -c 

wordParsing.o: wordParsing.c global.h project.h
	gcc -Wall -ansi -pedantic wordParsing.c -c 

main.o: main.c global.h project.h
	gcc -Wall -ansi -pedantic main.c -c 


