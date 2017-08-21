/* all global variables are declated in main.c file and start with g_ to distinguish them.
They are explained in main.c here they are defined so we can use them in other files */
extern struct labelStruct g_label[]; 
extern struct extLabelStruct g_ext_label[]; 
extern int g_labelNum, g_ext_labelNum;
extern unsigned short int g_code[];
extern unsigned short int g_data[];
extern codeEnum g_codeType[];
extern int g_IC, g_DC, g_codeLength, g_err;
extern struct cmdStruct cmds[];