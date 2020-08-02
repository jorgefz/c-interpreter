#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "printcmd.h"





//--------------------------------------------------------------


//GLOBALS

size_t MAX_LINE_SIZE = 100;

char **dtypes = (char *[]){"int", "float", "double", "char", "string"};
char fmt[5] = {'d', 'f', 'g', 'c', 's'};
int dtypeNum = 5;

/*
Given a data type as a string,
it returns the associated format specifier
e.g.	double -> g, int -> d
*/
char getFormatSpec(char *str)
{
	for(int i=0; i<dtypeNum; i++){
		if(strcmp(str, dtypes[i]) == 0 )
			return fmt[i];
	}
	return '\0';
}


char *isDataType(char *str)
{
	for(int i=0; i<dtypeNum; i++){
		if(strcmp(str, dtypes[i]) == 0 )
			return str;
	}
	return NULL;
}



/*
Inserts a string to another at index 'ind',
displacing the characters from ind forwards.
Dest should have enough memory for both.
*/
char *strins(char *dest, char *src, size_t ind)
{
	if(ind >= strlen(dest))
		return NULL;

	char temp[MAX_LINE_SIZE];
		
	//Copy chars before ind
	for(int i=0; i<(int)ind; i++)
		temp[i] = dest[i];

	//Insert new string
	for(int i=0; i<(int)strlen(src); i++)
        temp[i+ind] = src[i];

	//Copy rest of characters of dest
    for(int i=ind; i<(int)strlen(dest); i++)
        temp[i+strlen(src)] = dest[i];

	//Termination char
	temp[strlen(dest)+strlen(src)] = '\0';

	//Copy temp back to dest
	strcpy(dest, temp);

	return dest;
}

// Removes a character from a string
char *strdel(char *str, size_t ind)
{
	if(ind >= strlen(str))
		return NULL;
	for(size_t i=ind; i<strlen(str); i++)
		str[i] = str[i+1];
	return str;
}



//Inserts format specifier into printf string
char *insfmt(char *prnt, char *dtype)
{
	//Seek interior of printf quotes, before newline
	char *ptr = strstr(prnt, "\\n\"");

	//Generate format specifier
	char f = getFormatSpec(dtype);
	char fmt[] = {' ', '%', f, '\0'};

	//Insert fmt at pointer location
	strins(ptr, fmt, 0);

	return prnt;
}


//Inserts variable name into printf string
char *insvar(char *prnt, char *varName)
{
	//Remove disallowed characters
	rmch(varName, '"');
	rmch(varName, '\'');
	rmch(varName, '\\');
	rmch(varName, ';');

	//Move pointer to last parethesis
	char *ptr = prnt + strlen(prnt);
	while(*ptr != ')') ptr--;

	//Generate proper syntax for variable name
	char var[strlen(varName)+5];
	char temp[] = {',', ' ', '\0'};
	strcpy(var, temp);
	strcat(var, varName);

	//Insert varname at pointer location
	strins(ptr, var, 0);

	return prnt;
}



/* Inserts string onto printf */

char *instext(char *prnt, char *text)
{
	//Seek interior of printf quotes, before newline
	char *ptr = strstr(prnt, "\\n\"");
	//Remove double quotes from text to insert
	rmch(text, '"');
	//Insert text
	strins(ptr, text, 0);
	return prnt;
}



/*
Checks for input text to print.
Return values:
	0 	- text good
	1 	- not text
	-1	 - ERROR
*/
int isText(char **toks, int argnum, int *i, char *prnt)
{
	char *tok = toks[*i];

	// Check if token begins with quote
	if(tok[0] == '\"')
	{
		//Check if current token ends in double quote too
		//Whilst being longer than just "
		if(tok[strlen(tok)-1] == '\"' && strlen(tok) > 1){
			instext(prnt, tok);
			return 0;
		}

		//Search for next token ending in quote
		int found = *i;
		for(int j=*i+1; j<argnum; j++){
			if(toks[j][strlen(toks[j])-1] == '"'){
				found = j;
				break;
			}
		}
		
		//Exit if not found
		if(found == *i){
			printf(" String to print must be enclosed by double quotes\n");
			printf(" Usage: print \"hello world\" \n");
			return -1;
		}

		//Concatenate all toks from i to found, adding spaces in between
		char text[MAX_LINE_SIZE];
		text[0] = '\0';		//Must initialise string before strcatyh
		for(int j = *i; j<=found; j++){
			strcat(text, toks[j]);
			strcat(text, " ");
		}
		text[strlen(text)] = '\0';

		//Advance loop index
		*i = found;

		//Save text & return
		instext(prnt, text);
		return 0;
	}

	return 1;
}



/*
Checks whether print function is called.
Returns input if so, and NULL otherwise.
*/
char *isPrintCalled(char *input)
{
	// First word of input line, the command
	char command[MAX_LINE_SIZE];
	// Words after the command
	char arg[MAX_LINE_SIZE];

	int readNum = sscanf(input, " %s %[^\n] ", command, arg);
	if( readNum == 0)
		return NULL;
	else if( readNum > 0 && strcmp(command, "print") != 0 )
		return NULL;
	return input;
}


/*
Print command.
Converts input string into printf C code,
stored in pcmd and returned.
On wrong args, returns NULL
*/
char *print_cmd(const char *input, char *pcmd)
{
	// First word of input line, the command
	char command[MAX_LINE_SIZE];
	// Words after the command
	char arg[MAX_LINE_SIZE];
	// Number of args read
	int readNum;

	readNum = sscanf(input, " %s %[^\n] ", command, arg);
	if( readNum == 0)
		return NULL;

	else if( readNum != 0 && strcmp(command, "print") != 0 )
		return NULL;
	
	else if( readNum < 2 ){
		printf(" Not enough arguments for print command\n");
		printf(" Usage: print [data type] [variable name] \"text between quotes\"\n");
		printf(" e.g. print \"The number is\" int num \n");
		printf(" output-> printf(\"The number is  %%d\\n\", num);\n");
		return NULL;
	}

	// Tokenize arg by whitespace
	int argnum = strtokn(arg, ' ');
	char *tokens[argnum];
	char **toks = strsplit(tokens, arg, ' ');

	// initialise printf statement string
	char prnt[MAX_LINE_SIZE];
	strcpy(prnt, "printf(\"\\n\");");

	// Analyse arguments
	for(int i=0; i<argnum; i++)
	{
		char *tok = toks[i];

		// Is it quoted text to insert?
		int txtret = isText(toks, argnum, &i, prnt);
		
		//Wrong formatting in quoted text
		if(txtret < 0)
			return 0;
		//It's text
		else if(txtret == 0)
			continue;

		// Is it not a data type?
		else if( !isDataType(tok) )
		{
			printf(" Argument must be data type of variable to print\n");
			printf(" print [data type] [variable name]\n");
			printf(" -> [data type] '%s'\n", tok);
			return NULL;
		}

		//Check if data type is followed by a variable name
		// (not another data type or quoted string)
		else if (i == argnum-1 || isDataType(toks[i+1]) || toks[i+1][0] == '\"' )
		{
			printf(" Data type must be followed by variable name\n");
			printf(" print %s [variable name]\n", tok);
			if(i != argnum-1)
				printf(" -> %s %s\n", tok, toks[i+1]);
			else
				printf(" -> %s [variable]\n", tok);
			return NULL;
		}

		//FINALLY add variable & format specifier to printf
		else
		{
			insfmt(prnt, tok);
			insvar(prnt, toks[i+1]);
			//Skip next iteration (since its variable name)
			i++;
		}
	}

	printf("-> %s\n", prnt);
	strcpy(pcmd, prnt);
	return pcmd;
}










