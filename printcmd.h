

#ifndef _PRINT_CMD_H
#define _PRINT_CMD_H 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//GLOBALS

// Max size for print arguments string
size_t MAX_LINE_SIZE;

//Supported data types and format specifiers
char **dtypes;
char fmt[5];
int dtypeNum;

/*
Given a data type as a string,
it returns the associated format specifier
e.g.	double -> g, int -> d
*/
char getFormatSpec(char *str);

char *isDataType(char *str);



/*
Inserts a string to another at index 'ind',
displacing the characters from ind forwards.
Dest should have enough memory for both.
*/
char *strins(char *dest, char *src, size_t ind);

// Removes a character from a string
char *strdel(char *str, size_t ind);

//Inserts format specifier into printf string
char *insfmt(char *prnt, char *dtype);

//Inserts variable name into printf string
char *insvar(char *prnt, char *varName);

/* Inserts string onto printf */
char *instext(char *prnt, char *text);

/*
Checks for input text to print.
Return values:
	0 	- text good
	1 	- not text
	-1	 - ERROR
*/
int isText(char **toks, int argnum, int *i, char *prnt);


char *isPrintCalled(char *input);

/*
Return values
	0 	- Not print command
	1 	- Sucessfull
	-1 	- Error 
*/
char *print_cmd(const char *input, char *printcall);



#endif







