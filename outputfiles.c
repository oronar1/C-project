#include "project.h"
#include "global.h"

/* this file is responsible to write the .ob .ext and .ent files */

/* translate a decimal to array of octals 
receive: decimal
returns: array of octals */
void decimalToOctal (unsigned short int decimal, int octal[]) {
	int i=0;
	while (i<6) {
		octal[i]=decimal%8;
		decimal = decimal/8;
		i++;
	} 
}

/* prints decimal in octal format to a file.
receive: file, decimal, num
output: prints to file num digits in octal format. */
void printOctal (FILE *fp, unsigned short int decimal, int num) {
	int i; 
	int octal[6];
	decimalToOctal (decimal, octal);
	for (i=num-1; i>=0; i--)
		fprintf (fp, "%d", octal[i]);
}

/* prints the object file */
void writeObject (FILE *fp2)  {
	int i;
	fprintf (fp2, "\t");
	printOctal (fp2,g_codeLength,4); /* write code length in 4 digits*/
	fprintf (fp2, " ");
	printOctal (fp2,g_DC,4); /* write data length in 4 digits */
	fprintf (fp2, "\n");
	for (i=0; i<g_codeLength; i++) { /* loop to print code */
		printOctal (fp2, START_ADD+i,4); /* address in 4 digits */
		fprintf (fp2, "\t");
		printOctal (fp2,g_code[START_ADD+i],6); /* machine code in 6 digits */
		fprintf (fp2, "\t");
		switch (g_codeType[START_ADD+i]) { /* a-Absolute r-Relocatable e-external */
		case ABSOLUTE:
			fprintf (fp2, "a\n");
			break;
		case RELOCATABLE:
			fprintf (fp2, "r\n");
			break;
		case EXTERNAL:
			fprintf (fp2, "e\n");
			break;
		}
	} 
	for (i=0; i<g_DC; i++) { /* loop to print data */
		printOctal (fp2, START_ADD+g_codeLength+i,4); /* 4 octal digits for address */
		fprintf (fp2, "\t");
		printOctal (fp2,g_data[i],6); /* 6 octal digits for data */
		fprintf(fp2, "\n");
	}
	fclose(fp2); 
}

void writeExternal (FILE *fp) { /* write the .ext file */
	int i,j;
	for (i=0; i<g_ext_labelNum; i++) { /* go over external labels */
		for (j=0; j<g_ext_label[i].extAddressNum;j++) { /* for each label track all positions */
			fprintf(fp, "%s\t", &g_ext_label[i].word.line[0]);
			printOctal(fp,g_ext_label[i].extAddress[j], 4); /* and write their address to file */
			fprintf(fp,"\n");
		}
	}
}

void writeEntry (FILE *fp) { /* write the .ent file */
	int i;
	for (i=0; i<g_labelNum; i++) { /* go over all labels */
		if (g_label[i].entry) { /* process only those marked with .entry */
			fprintf(fp, "%s\t", &g_label[i].word.line[0]);
			if (g_label[i].type==CODE) /* if it's CODE, write address */
				printOctal(fp,g_label[i].address+START_ADD, 4);	
			else /* if it's data, write address after CODE ends */
				printOctal(fp,g_label[i].address+START_ADD+g_codeLength, 4);
			fprintf(fp,"\n");
		}
	}
}