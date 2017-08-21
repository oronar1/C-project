#include "project.h"
/* project made by:
Ariel Bar-Itzhack ID 014210520
Yuval Edelman ID 066372368

project goal is to translate assembly files to octal/binary files which 
are used by the linker program for assembly.
Input files are given in command arguments i.e. maman14 ps cs a rs
the program will append .as to the file names for input.
if tranlation encounters error it will send them to stderr and stop the translation at end of file.
if no errors it will produce 3 files for each input file:
1) file.ob - object code
2) file.ent - entries label codes
3) file.ext - external label codes

remark: we didn't use malloc for this Maman in order to make it as simple as possible.
Hence everything is stored in arrays */

/* global variables are defined here they start with g_ to distinguish them */
struct labelStruct g_label[MAX_LABELS]; /* label array */
struct extLabelStruct g_ext_label[MAX_LABELS]; /* external label array */
int g_labelNum=0, g_ext_labelNum=0; /* actual number of labels & external labels */
unsigned short int g_code[MACHINE_RAM]; /* array to store machine code */
unsigned short int g_data[MACHINE_RAM]; /* array to store machine data (numbers and strings) */
codeEnum g_codeType[MACHINE_RAM]; /* type of code ABSOLUTE, RELOCATABLE, EXTERNAL */
int g_IC=START_ADD, g_DC=0; /* Instruction Counter and Data counter */
int g_codeLength; /* total CODE length */
int g_err; /* have we reached an error in file? */
struct cmdStruct cmds[] = {
	/* table of all commands which holds: string, code, number of arguments, permittable params in source destination.
	the 4 permittable params are: immediate, direct, indirect, register. 1 means permitted 0 means not permitted. */
	{"mov", 0, 2, {{1,1,1,1}, {0,1,1,1}}},
	{"cmp", 1, 2, {{1,1,1,1}, {1,1,1,1}}}, 
	{"add", 2, 2, {{1,1,1,1}, {0,1,1,1}}}, 
	{"sub", 3, 2, {{1,1,1,1}, {0,1,1,1}}}, 
	{"ror", 4, 2, {{1,1,1,1}, {0,1,1,1}}}, 
	{"shr", 5, 2, {{1,1,1,1}, {0,1,1,1}}}, 
	{"lea", 6, 2, {{0,1,0,0}, {0,1,1,1}}}, 
	{"inc", 7, 1, {{0,0,0,0}, {0,1,1,1}}}, 
	{"dec", 8, 1, {{0,0,0,0}, {0,1,1,1}}}, 
	{"jmp", 9, 1, {{0,0,0,0}, {0,1,1,0}}}, 
	{"bne", 10, 1, {{0,0,0,0}, {0,1,1,0}}}, 
	{"red", 11, 1, {{0,0,0,0}, {0,1,1,1}}}, 
	{"prn", 12, 1, {{0,0,0,0}, {1,1,1,1}}}, 
	{"jsr", 13, 1, {{0,0,0,0}, {0,1,1,0}}}, 
	{"rts", 14, 0, {{0,0,0,0}, {0,0,0,0}}}, 
	{"hlt", 15, 0, {{0,0,0,0}, {0,0,0,0}}}
};

int main (int argc, char *argv[]) {
	FILE *fp1, *fp2; /* fp1 for read fp2 for write */
	int i;
	char fileName[4][50]; /* array to store up to 4 file names */
	if (argc == 1) {
		fprintf (stderr, "No input file.\n");
		return 1;
	}
	for (i=1;i<argc;i++) { /* for all files in command line */
		strcpy (&fileName[0][0],*(argv+i)); 
		strcat (&fileName[0][0], EXT_READ); /* append .as to file name */
		strcpy (&fileName[1][0],*(argv+i));
		strcat (&fileName[1][0], EXT_OBJECT); /* append .ob to file name */
		strcpy (&fileName[2][0],*(argv+i));
		strcat (&fileName[2][0], EXT_EXTERN); /* append .ext to file name */
		strcpy (&fileName[3][0],*(argv+i));
		strcat (&fileName[3][0], EXT_ENTRY); /* append .ent to file name */
		printf ("Input filename: %s\n", &fileName[0][0]);
		printf ("Object filename: %s\n", &fileName[1][0]);
		printf ("Extern filename: %s\n", &fileName[2][0]);
		printf ("Entry filename: %s\n", &fileName[3][0]);
		if ((fp1 = fopen(&fileName[0][0], "r")) == NULL) { /* open .as file for read */
			fprintf (stderr, "can't open %s\n", &fileName[0][0]);
			return 1; 
		} 
		parsing (fp1); /* parse the file */
		if (g_err)  /* if we reached an error exit */
			continue;
		if ((fp2 = fopen(&fileName[1][0], "w")) == NULL) { /* open .ob file to write */
			fprintf (stderr, "can't open %s\n", &fileName[1][0]);
			return 1; 
		} 
		writeObject(fp2); /* write .ob file content */
		if ((fp2 = fopen(&fileName[2][0], "w")) == NULL) { /* open .ext file to write */
			fprintf (stderr, "can't open %s\n", &fileName[1][0]);
			return 1; 
		} 
		writeExternal(fp2); /* write .ext file content */
		if ((fp2 = fopen(&fileName[3][0], "w")) == NULL) { /* open .ent file to write */
			fprintf (stderr, "can't open %s\n", &fileName[1][0]);
			return 1; 
		} 
		writeEntry(fp2); /* write .ent file content */
	}
	getchar();
	return 0;
}


