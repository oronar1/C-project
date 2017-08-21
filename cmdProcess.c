#include "project.h"
#include "global.h"

/* this file is responsible to interpert commands */

/* function to process command 
receives:
	line[], lenLine, *pos - current line
	cmdIndex - index of command
	pass - 1st or 2nd pass */
int cmdProcess (char line[], int lenLine, int *pos, int cmdIndex, int pass) {
	struct wordStruct arg1; 
	int count=1; /* count is to check how many words to increase IC */
	int err=0; /* did we reach an error? */
	if (pass==2) {
		g_code[g_IC]=cmds[cmdIndex].code<<12; /* on 2nd pass insert the command code to g_code */
		g_codeType[g_IC]=ABSOLUTE; /* and mark is as absolute */
	}
	if (cmds[cmdIndex].oper > 0) { /* if we have 1 or 2 arguments for command */
		parseItem (&arg1, line, lenLine, pos); /* read 1st argument */
		if (arg1.type!=REGISTER) count++; /* registers are not counted for occopying space */
		if (pass==2) err=cmdProcess2(arg1, cmdIndex, 1); /* on 2nd pass process 1st argument */
		if (err>0) return err;
	}
	if (cmds[cmdIndex].oper == 2) { /* if we have 2 arguments */
		parseItem (&arg1, line, lenLine, pos); /* read next item */
		if (arg1.type==COMMA) { /* which should be comma */
			parseItem (&arg1, line, lenLine, pos); /* and then read next argument */
			if (arg1.type!=REGISTER) count++; /* registers are not counted for occopying space */
			if (pass==2) err=cmdProcess2(arg1, cmdIndex, 2); /* on 2nd pass process 2nd argument */
			if (err>0) return err;
		}else return 2;
	}
	g_IC += count; /* update IC pass 2 item #9 */
	return 0;
}

/* function to process command on 2nd pass 
receive:
	arg1 - word
	cmdIndex - command index
	argPass - is it 1st or 2nd argument */
int cmdProcess2 (struct wordStruct arg1, int cmdIndex, int argPass) {
	int i, dest=1; /* assume argument is destination */
	if (cmds[cmdIndex].oper==2 && argPass==1) /* if 2 arguments and 1st pass then assume argument is source */
		dest = 0;
	if (arg1.type==IMMEDIATE) { /* handle immediate */
		if (!cmds[cmdIndex].permit[dest][0]) return 1; /* check if permitted */
		g_code[g_IC+argPass]=arg1.decimal; /* update code array */
		g_codeType[g_IC+argPass]=ABSOLUTE; /* and mark it as absolute */
	} else if (arg1.type==OTHER || arg1.type==RELATIVE) { /* hand direct(OTHER) and relative */
		i=labelSearch(&arg1.line[0]); /* search for label */
		if (arg1.type==OTHER) { /* handle direct (OTHER) */
			if (!cmds[cmdIndex].permit[dest][1])return 1; /* check if permitted */
			if (dest)
				g_code[g_IC]|=00010; /* destination code */
			else
				g_code[g_IC]|=01000; /* source code */
			if (i>-1){ /* if there is a label */
				g_codeType[g_IC+argPass]=RELOCATABLE; /* it's relocatable */
				if (g_label[i].type == CODE) { /* type CODE */
					g_code[g_IC+argPass]=g_label[i].address; /* set CODE address */
				}else { /* type DATA */
					g_code[g_IC+argPass]=START_ADD + g_codeLength +g_label[i].address; /*  set DATA address */
				}
			} 
		} else { /* handle relavite */
				if (!cmds[cmdIndex].permit[dest][2]) return 1; /* check if permitted */
				if (dest)
					g_code[g_IC]|=00030; /* source code */
				else
					g_code[g_IC]|=03000; /* destination code */
				if (i>-1) { /* if label found */
					g_codeType[g_IC+argPass]=ABSOLUTE; /* mark it absolute */
					if (g_label[i].type == CODE) {
						g_code[g_IC+argPass]=g_label[i].address-g_IC; /* set CODE address */
					} else { /* DATA */
						g_code[g_IC+argPass]=START_ADD + g_codeLength + g_label[i].address-g_IC; /* set DATA address */
					}
				} 
		} 
		if (i==-1){ /* if label wasn't found */
			i=ext_labelSearch(&arg1.line[0]); /* check if it's an external label */
			if (i==-1) return 3; /* handle error: label not found */
			g_code[g_IC+argPass]=0; 
			g_codeType[g_IC+argPass]=EXTERNAL; /* mark it external */
			g_ext_label[i].extAddress[g_ext_label[i].extAddressNum]=g_IC+argPass; /* store external label address */
			(g_ext_label[i].extAddressNum)++;
		}
	} else if (arg1.type==REGISTER) { /* handle register */
		if (!cmds[cmdIndex].permit[dest][3]) return 1; /* check if permittable */
		if (dest) { /* handle destination */
			g_code[g_IC] |= 00040; /* store code */
			g_code[g_IC] |= arg1.reg; /* and register number */
		}else { /* handle source */
			g_code[g_IC] |= 04000;/* store code */
			g_code[g_IC] |= (arg1.reg<<6);  /* and register number */
		}
	} else return 4; /* return error: bad or missing argument */
	return 0;
} 

