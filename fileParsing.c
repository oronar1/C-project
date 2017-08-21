#include "project.h"
#include "global.h"

/* this file is responsible for main algorith 1st and 2nd pass */

/* function to send error messages
receive: error message, line with error
output: line with error & error message to stderr
sets g_err to 1 to mark that error was found */
void printErr (char line[], int lenLine, char *msg) { 
	int i; 
	for (i=0; i<lenLine; i++)
		putc (line[i],stderr);
	putc ('\n',stderr); 
	g_err=1;
	fprintf(stderr, "%s", msg);
}

/* search for command in array of commands and returns index, -1 means not found */
int cmdSearch (char cmd[]) { 
	int i;
	for (i=0; i<=15;i++)
		if (strcmp(cmd, cmds[i].name) == 0)
			return i;
	return -1;
}

/* search for label in array of labels and returns index, -1 means not found */
int labelSearch (char str[]) {
	int i;
	for (i=0; i<g_labelNum;i++)
		if (strcmp(str, &g_label[i].word.line[0]) == 0)
			return i;
	return -1;
}

/* search for external label in array of external labels and returns index, -1 means not found */
int ext_labelSearch (char str[]) {
	int i;
	for (i=0; i<g_ext_labelNum;i++)
		if (strcmp(str, &g_ext_label[i].word.line[0]) == 0)
			return i;
	return -1;
}

/* main algorith 1st and 2nd pass 
receive: file
output: updates labels, code table, data table etc. */
void parsing (FILE *fp1) {
	char line[LINE_LENGTH]; /* we will store 1 line buffer here */
	struct wordStruct word, wordTemp; /* word objects typically hold number, lablel, command etc. */
	int lenLine, pos ; /* lenLine - length of line. pos - current position in line */
	int i, x=0;  /* i is loop index. x reads next char from file */
	int isSymbol; /* flag is it symbol ? */
	g_labelNum=0; /* reset global number of labels to zero */
	g_ext_labelNum=0; /* reset global number of external labels to zero */
	g_IC=START_ADD; g_DC=0; /* reset global Instruction and Data Counters 1st pass item #1 */
	g_codeLength=0; /* reset global Code Length */
	fprintf (stderr, "Pass one\n");
	while (x!=EOF) { /* main loop goes on all file */
		i=0;
		x = getc(fp1);
		while (x != '\n' && x != EOF) { /* reads a line 1st pass item #2 */
			if (i<=LINE_LENGTH-1) line[i] = (char) x; /* and put it in char array */
			i++;
			x = getc(fp1);
		}
		if (i<=LINE_LENGTH-1) 
			lenLine = i;
		else /* if line is longer that maximum allowed, truncate it */
			lenLine=LINE_LENGTH-1;
		if (i==0 || line[0]==';') {pos=lenLine; continue;} /* skip remarks */
		pos = 0; 
		while (pos < lenLine) { /* secondary loop goes over all line */
			isSymbol=0;
			parseItem (&word, line, lenLine, &pos); /* parse a word */
			if (word.type==LABEL){ /* check if it's a label 1st pass item #3 */
				isSymbol=1; /* set flag 1st pass item #4 */
				wordTemp = word; /* store label in temp storage for later use */
				parseItem (&word, line, lenLine, &pos); /* parse a word */
			}
			if (word.type==DATA_N || word.type==DATA_S) { /* is it .data or .string? 1st pass item #5 */
				if (isSymbol) { /* if it's symbol put in table 1st pass item #6 */
					g_label[g_labelNum].word=wordTemp;
					g_label[g_labelNum].type=DATA;
					g_label[g_labelNum].entry=0;
					g_label[g_labelNum].address=g_DC; /* and it's value will be DC */
					g_labelNum++;
				}
				if (word.type==DATA_S) { /* find data type 1st pass item #7. string part ahead*/
					parseItem (&word, line, lenLine, &pos); 
					if (word.type==STRING) { /* handle string data */
						i=0; 
						while (word.line[i] != '\0') {
							g_data[g_DC+i] = word.line[i]; /* store it in Data array */
							i++;
						}
						g_data[g_DC+i] = '\0';
						g_DC += i+1; /* update DC with string length + 1 for '\0' */
					} else if (word.type!=SPACE)
						printErr(line, lenLine,"not a string!\n" ); /* handle error: not a string */
				}else { /* if it's not a string then it's a list of number */
					parseItem (&word, line, lenLine, &pos);
					while (word.type==NUMBER) { /* loop to handle list of numbers */
						g_data[g_DC] = word.decimal; /* store it in DATA array */
						g_DC++; /* increase DC by one for each number */
						parseItem (&word, line, lenLine, &pos);
						if (word.type!=COMMA) break; /* numbers should be separated by comma */
						parseItem (&word, line, lenLine, &pos);
					}
					if (word.type != SPACE)
						printErr(line, lenLine,"missing ',' or not a number!\n" ); /* handle error: not a number */
				} 
				pos = lenLine; continue; /* end of pass 1 item #7, back to stage 2 */
			}
			if (word.type==ENTRY || word.type==EXTERN) { /* is it .extern or .entry pass 1 item #8 */
				if (word.type==EXTERN) { /* if it's external */
					parseItem (&word, line, lenLine, &pos);
					if (isLabel(&word)) { /* and there is valid label */
						g_ext_label[g_ext_labelNum].word=word; /* store in external label array, pass 1 item #9 */
						g_ext_label[g_ext_labelNum].extAddressNum=0; 
						g_ext_labelNum++;
					} else if (word.type != SPACE)
						printErr(line, lenLine,"not a label!\n" ); /* handle error: not a label */
				}
				pos = lenLine; continue; /* pass 1 item #10 back to stage 2 */
			}
			if (isSymbol) { /* if there is a symbol, pass 1 item #11 */
				g_label[g_labelNum].word=wordTemp; /* put it in label array */
				g_label[g_labelNum].type=CODE;
				g_label[g_labelNum].address=g_IC; /* and it's address will be IC */
				g_labelNum++;
			}
			if (word.type != SPACE) { /* ignore empty spaces */
				i=cmdSearch(&word.line[0]); /* search command table pass 1 item #12 */
				if (i>-1) 
					cmdProcess(line, lenLine, &pos,i,1); /* this method will update IC pass 1 item #13 */
				else {
					pos=lenLine; printErr(line, lenLine,"not a command!\n" ); 
				}
			}
		} /* end of loop to process line */
	} /* end of loop to process file pass 1 item #14 */
	rewind(fp1); /* reset to start of file to prepare for pass 2 */
	fprintf (stderr, "Pass two\n");
	g_codeLength=g_IC-START_ADD;x=0; /* we can now calculate CODE Length */
	g_IC=START_ADD; /* reset IC pass 2 item #1 */
	while (x!=EOF) { /* main loop for pass 2 reads all file */
		i=0;
		x = getc(fp1);
		while (x != '\n' && x != EOF) { /* reads a line pass 2 item #2 */
			if (i<=LINE_LENGTH-1) line[i] = (char) x;
			i++;
			x = getc(fp1);
		}
		if (i<=LINE_LENGTH-1) 
			lenLine = i;
		else /* if item is too long truncate it */
			lenLine=LINE_LENGTH-1;
		if (i==0 || line[0]==';') {pos=lenLine; continue;} /* ignore remarks */
		pos = 0; 
		while (pos < lenLine) { /* secondary loop go over all line */
			parseItem (&word, line, lenLine, &pos);
			if (word.type==LABEL) /* if it's a label, skip it pass 2 item #3 */
				parseItem (&word, line, lenLine, &pos);
			if (word.type==DATA_S || word.type==DATA_N) /* if it's .data or .string go to step 2 pass 2 item #4 */
				{pos = lenLine; continue;}
			if (word.type==ENTRY || word.type==EXTERN) { /* is it entry or extern? pass 2 item #5 */
 				if (word.type == ENTRY) { /* handle entry pass 2 item #6 */
					parseItem (&word, line, lenLine, &pos);
					i = labelSearch(&word.line[0]); /* if label exists */
					if (i>-1){
						g_label[i].entry=1;	/* mark it as entry */
					} else
						printErr(line, lenLine,"cannot find entry label !\n" ); /* handle error: entry label */
				}
				pos = lenLine; continue; /* and back to stage 2 */
			}
			i=cmdSearch(&word.line[0]); /* search for command */
			if (i>-1) {
				i=cmdProcess(line, lenLine, &pos,i,2); /* function to process pass 2 item #7,8,9 */
				switch (i) { /* if cmdProcess returns an error, handle it */
					case 1:
						printErr(line, lenLine,"argument not permitted by command!\n" );
						break;
					case 2:
						printErr(line, lenLine,"comma expected!\n" );
						break;
					case 3:
						printErr(line, lenLine,"label not found!\n" );
						break;
					case 4:
						printErr(line, lenLine,"bad or missing argument!\n" );
						break;
				}	
			}

		} /* end of secondary loop process line pass 2 item #10 */
	} /* end of main loop process file in pass 2 */
}	/* end of parsing function */
