#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define LINE_LENGTH 80 /* maximum chars per line */
#define MAX_LABELS 100 /* maximum number of labels */
#define MAX_EXTERNAL_ADDRESSES 20 /* max pointers back to external label positions */
#define MACHINE_RAM 2000 
#define START_ADD 0 /* offset for CODE start address */
typedef enum {LABEL = 0, DATA_N, DATA_S, EXTERN, ENTRY, COMMA, CMD, NUMBER, STRING, SPACE, IMMEDIATE, RELATIVE,
	REGISTER, OTHER} wordType;
/* 
DATA_N = .data with number after
DATA_S = .data with string after
EXTERN = .extern
ENTRY = .entry
CMD = any of 15 commands
SPACE = any white spaces (space, tab newline)
IMMEDIATE = number starts with #
RELATIVE label starts with *
REGISTER = r1..r8
OTHER = non of the above */
typedef enum {DATA = 0, CODE} symbolType; 
typedef enum {ABSOLUTE=0, RELOCATABLE, EXTERNAL} codeEnum;
struct wordStruct { /* we read text from file and put it into this structure */
	char line[LINE_LENGTH]; /* holds text */
	wordType type; /* classification of text such as label, number string, command etc. */
	int decimal; /* if it is a number how much is its value */
	int reg; /* if it's a register what is register number */
};
struct labelStruct { /* we store labels in this structure */
	struct wordStruct word; /* copy of WordStruct that was created from text */
	symbolType type; /* is it CODE or DATA */
	int address; /* address of label */
	int entry; /* if it's an .entry label put 1 else 0 */
};
struct extLabelStruct { /* we store external labels in this structure */
	struct wordStruct word; /* copy of WordStruct that was created from text */
	symbolType type; /* is it CODE or DATA */
	int	extAddress[MAX_EXTERNAL_ADDRESSES]; /* max pointers back to external label positions */
	int extAddressNum; /* actual number of pointers back to external label positions */
};
struct cmdStruct { /* stores commands such as mov, cmp, inc etc. */
	char name[4]; /* 3 chars + '0' */
	int code; /* code between 0 and 15 */
	int oper; /* how many operands for this command (0-2) */
	int permit[2][4]; /* table of 2*4 permittable source/destinations operands: immediate, direct, indirect, register */
};

#define EXT_READ ".as" /* extention for read assembly file */
#define EXT_OBJECT ".ob" /* extention of write object file */
#define EXT_EXTERN ".ext" /* extention of write external file */
#define EXT_ENTRY ".ent" /* extention of write entry file */

/* these are just headers will be explained where they are implemented */
void parsing (FILE *fp1);
int cmdProcess (char line[], int lenLine, int *pos, int cmdIndex, int pass);
int cmdProcess2 (struct wordStruct arg1, int cmdIndex, int argPass);
void writeObject (FILE *fp2);
void writeExternal (FILE *fp);
void writeEntry (FILE *fp);
int isNumber(struct wordStruct *word, int *number);
int isLabel (struct wordStruct *word);
void parseItem (struct wordStruct *word, char line[], int lenLine, int *pos);
int cmdSearch (char cmd[]) ;
int labelSearch (char str[]);
int ext_labelSearch (char str[]);
 