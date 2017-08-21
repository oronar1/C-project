#include "project.h"
/* this file is responsible to interpert text and check if it's label, number, string etc. */

/* parses next item on line and interperts it
receive: char line[], int lenLine, int *pos - line
returns: *word - interpertation */
void parseItem (struct wordStruct *word, char line[], int lenLine, int *pos) {
	int j, start; /* j is number of chars in word and start is which position in line it starts */
	wordType type=OTHER; /* default if not recognized */
	while (*pos<lenLine && (line[*pos] == ' ' || line[*pos]=='\t')) /* skip white spaces */
		(*pos)++;
	j=0; start=-1; 
	while (*pos<lenLine && (line[*pos] != ' ' && line[*pos]!='\t' && line[*pos]!=',')){ /* util you reach white space or comma */
		if (start==-1) start=*pos; /* initialize start position */
		if (j==0 && line[*pos]=='#') /* if it starts with # then it is immediate */
			type = IMMEDIATE;
		else if (j==0 && line[*pos]=='*') /* if it starts with * then it's indirect */
			type = RELATIVE;
		else if (j==0 && line[*pos]=='"') /* if it starts with " then it's a string */
			type = STRING;
		else {
			word->line[j] = line[*pos]; /* store char if not one of these: # * "			*/
			j++;
		} 
		(*pos)++; /* process next char */
	} /* end of loop: we have reached white space or comma */
	if (j==0 && line[*pos] == ',') { /* if it's a comma store it in word */
		word->line[j] = line[*pos];
		(*pos)++; j++;
	}
	word->line[j]=(char)'\0'; /* append '\0' to the string */
	if (j == 0) /* no char was found */
		word->type=SPACE; /* identify as SPACE */
	else if (j==2 && word->line[0] == 'r' && word->line[1] >='0' && word->line[1]<='7') {
		/* word is 2 chars 1st is r and 2nd is number between 0 and 7 */
		word->type=REGISTER; /* identify as REGISTER */
		word->reg=(word->line[1] - '0'); /* store register number */
	} else if (start == 0 && word->line[j-1] == ':' && isLabel(word)) { 
		/* did we find label that starts on position 0 and end with :	*/
		word->type=LABEL; /* identify as LABEL */
		word->line[j-1]='\0'; /* replace : with \0 as labels are stored without the : */
	}else if (type==IMMEDIATE && isNumber(word,&word->decimal)) /* if it's immediate and number */
		word->type=IMMEDIATE; /* identify as IMMEDIATE */
	else if (isNumber(word, &word->decimal)) /* if it number */
		word->type=NUMBER; /* identify as NUMBER */
	else if (type==RELATIVE && isLabel(word)) /* if it's relative and valid label */
		word->type=RELATIVE; /* identify as RELATIVE */
	else if (type==STRING && j>=2 && word->line[j-1]=='"') { 
		/* if it's string with at least 2 chars and ends with "		*/
		word->type=STRING; /* identify as STRING */
		word->line[j-1]='\0'; /* do not store the " as strings contain only the inside */
	}else if (strcmp (&word->line[0], ".data")==0) 
		word->type=DATA_N; /* .data identify as DATA_N */
	else if (strcmp (&word->line[0], ".string")==0)
		word->type=DATA_S; /* .string identify as DATA_S */
	else if (strcmp (&word->line[0], ".entry")==0)
		word->type=ENTRY; /* .entry identify as ENTRY */
	else if (strcmp (&word->line[0], ".extern")==0)
		word->type=EXTERN; /* .extern identify as EXTERN */
	else if (word->line[0] == ',') /* identify COMMA */
		word->type=COMMA;
	else if (cmdSearch(&word->line[0])>-1) /* search command list */
		word->type=CMD; /* if found identify as CMD */
	else 
		word->type=OTHER; /* nothing found. could be a label without : at the end */
}

/* this function checks to see if valid number
receive: word
returns: 1 if a number, also caculates decimal value */
int isNumber(struct wordStruct *word, int *decimal){
	int minus=0, digit=1, pos=0, pos_start, pos_end;
	if (word->line[pos] == '-') { /* handle negative sign */
		minus = 1;
		(pos) ++;
	} else if (word->line[pos] == '+')  /* handle plus sign */
		(pos) ++;
	if (!(isdigit(word->line[pos]))) return 0; /* check if 1st char is digit */
	pos_start = pos;
	while (isdigit(word->line[pos])) /* skip digits */
		pos++;
	if (word->line[pos] !='\0')return 0; /* reached end */
	pos_end= pos-1;
	*decimal = 0;
	for (pos = pos_end; pos >= pos_start; pos--) { /* loop to caculate decimal */
		*decimal += (word->line[pos] - '0') * digit;
		digit *=10;
	}
	if (minus) *decimal *= -1; /* accomodate for minus sign */
	return 1;
}

/* check if word is label 
receive: word
returns: 1 if label */
int isLabel (struct wordStruct *word) {
	int pos=0;
	if (!(isalpha(word->line[pos]))) return 0; /* 1st char is alphanumeric */
	pos++;
	while (pos <= 30 && isalnum(word->line[pos])) /* skip alphanumerics up to 30 chars */
		pos++;
	/* check if it ends with ':\0' or '\0' */
	if ((word->line[pos] == ':' && word->line[pos+1] == '\0') || word->line[pos] == '\0') return 1;
	return 0;
}