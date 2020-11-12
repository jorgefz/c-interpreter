

/*
	==================  SCLI ===================
		=== Simple C Language Interpreter ===

	HISTORY
	Version 1.0 - 30/07/2020
		- Added main interpreter
		- Added commands:
			reset, exit, clear, help

	Version 1.1 - 01/08/2020
		- Added option to choose compiler system call
		via command line arguments.
		e.g.	scli -c "ggc -Wall -Wextra"

	Version 1.2  - 02/08/2020
		- Added print command

	Version 1.3 - 06/08/2020
		- Added rline command
		- Added show command
		- Added support for Linux & MacOS systems

	Version 1.4 - 02/10/2020
		- Added argc and argv to main
		- Added command -a/--args that sets the
		input arguments for the C file to interpret.

	Version 1.5 - 12/11/2020
		- Updated arglib


	TO-DO
		- Command line arguments:
			> Choose output filename 
		- Multi platform support for compiling to
			executables
		- Command to navigate file pointer above main
			to write includes, macros, structs, and functions

		- Get whether compilation failed with
			return value of system call.
			SUCCESS: ret = 0
			FAILED: ret = 1

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "printcmd.h"
#include "arglib.h"

// OS dependent global variables
#if defined (_WIN32)
const int OS_WIN = 1;
const int OS_UNIX = 0;
const char *EXENAME = "outc.exe";
const char *CALLCMD = "outc";
const char *DELCMD = "del ";

#elif defined (__APPLE__) || defined (__linux__)
const int OS_WIN = 0;
const int OS_UNIX = 1;
const char *EXENAME = "outc";
const char *CALLCMD = "./outc";
const char *DELCMD = "rm ";

#endif


const char *VERSION = "V 1.4";
const char *VDATE = "02/10/2020";

const char *CPATH = "outc.c";

typedef unsigned short ushort;



//--------------------------------------------------------

void printFileLines()
{
	FILE *read = fopen(CPATH,"r");
	fseek(read, 0, SEEK_SET);
	char c;
	while( (c = fgetc(read)) != EOF )
		printf("%c",c);
	fclose(read);
}


/* Create beginning of basic main program */
void createMain(FILE *outc)
{
	//Include standard libraries
	fprintf(outc, "#include <stdio.h>\n");
	fprintf(outc, "#include <stdlib.h>\n\n");
	// Initiate main function
	fprintf(outc, "int main(int argc, const char *argv[]){\n\n");
}

/* Moves stream pointer to the line before 'return 0;' */
void seekMiddle(FILE *outc)
{
	//Return just before ending statements of main
	fseek(outc, 0, SEEK_CUR);
	fseek(outc, -1, SEEK_END);


	while(1){
		char c = fgetc(outc);
		if(c == '}')
			break;
		else if(c == '\n' && OS_WIN == 1)
			fseek(outc, -3, SEEK_CUR);
		else
			fseek(outc, -2, SEEK_CUR);
	}

	//Add 1 if Windows for \r
	int move = sizeof("return 0;\n}") + OS_WIN;
	fseek(outc, -move, SEEK_CUR);
	fseek(outc, 0, SEEK_CUR);
}

/* Write end of basic main
(return 0, close curly brackets) */
void endMain(FILE *outc)
{
	fprintf(outc, "return 0;\n");
	fprintf(outc, "}\n");
}

/*
Writes return statement with an input line,
and rewrites it below.
*/
void writeln(FILE *outc, const char *line)
{
	seekMiddle(outc);
	/*Switch reading -> writing */
	fseek(outc, 0, SEEK_CUR);

	fprintf(outc, "\n%s\n", line);
	endMain(outc);

	/* Switch writing -> reading */
	fflush(outc);
	seekMiddle(outc);

	/* Switch reading -> writing */
	fseek(outc, 0 , SEEK_CUR);
}

int checkFile(const char *path)
{
	FILE *f = fopen(path, "r");
	if(!f)
		return 0;
	fclose(f);
	return 1;
}

/* Compile & execute program */
void compile(const char *compilerCall, const char *args)
{
	char call[strlen(compilerCall)+20];
	strcpy(call, compilerCall);
	strcat(call, " ");
	strcat(call, CPATH);
	strcat(call, " -o outc");
	if(checkFile(CPATH) == 1){
		system(call);
	}
	if(checkFile(EXENAME) == 1){
		char execCall[strlen(CALLCMD)+strlen(args)+20];
		strcpy(execCall, CALLCMD);
		strcat(execCall, args);
		system(execCall);
	}
}


void delIfExist(const char *path)
{
	if(checkFile(path) == 1)
	{
		char del[strlen(path)+5];
		strcpy(del, DELCMD);
		strcat(del, path);
		system(del);
	}
}


FILE *rline(FILE *outc, int *lwritten)
{
	//Don't remove lines if there are no lines!
	//Otherwise, main() declaration is removed
	if(*lwritten == 0)
		return outc;

	/* writing -> reading */
	fflush(outc);
	seekMiddle(outc);

	//Back out one char to avoid previous newline
	fseek(outc, -2, SEEK_CUR);
	//Now we are at ';' of statement before return 0;

	/*
	Moving 3 positions accounts for '\r\n' in Windows,
	and for fgetc jump;
	but MacOS and Linux only use '\n' so the value
	should be -2 for those systems.
	*/

	//Loop backwards until beginning of previous line
	int rmchars = 0;

	char c = ' ';
	while( c != '\n'){
		c = fgetc(outc);
		fseek(outc, -2, SEEK_CUR);
		rmchars++;
	}
	fseek(outc,1+OS_UNIX, SEEK_CUR);
	rmchars -= OS_UNIX;

	// Reading -> writing
	fseek(outc, 0, SEEK_CUR);

	//Subtitute everything below with whitespaces
	int move = rmchars + sizeof("\nreturn 0;\n}\n");
	for(int i=0; i<move; i++){
		fprintf(outc, " ");
	}

	// Writing -> reading
	fflush(outc);

	//Roll back so new input line overwrites whitespaces
	fseek(outc, -move-1, SEEK_CUR);
	fseek(outc, 0, SEEK_CUR);
	fprintf(outc, "\n");
	
	//Rewrite end of main
	endMain(outc);

	fflush(outc);
	seekMiddle(outc);

	//Update line counter
	(*lwritten)--;
	return outc;
}

/*
Checks if input line is command
Returns file pointer if sucessfull
and NULL otherwise
*/
FILE *checkCommands(char *line, FILE *outc, char *comp, char *args, int *lwritten)
{
	// Exit program
	if(strcmp(line, "exit") == 0){
		fclose(outc);
		delIfExist(CPATH);
		delIfExist(EXENAME);
		exit(0);
	}
	
	// Reset c file
	else if(strcmp(line, "reset") == 0)
	{
		fclose(outc);
		//Reset contents with new instance
		FILE *temp = fopen(CPATH, "w");
		fclose(temp);
		//Redo file
		FILE *new = fopen(CPATH, "w+");
		createMain(new);
		endMain(new);
		seekMiddle(new);
		//Delete executable
		delIfExist(EXENAME);
		//Clear
		checkCommands("clear", outc, comp, args, lwritten);
		//Reset lines counter
		*lwritten = 0;
		return new;
	}
	// Clear
	else if(strcmp(line, "clear") == 0)
	{
		for(int i=0; i<20; i++)
			printf("\n");
		return outc;
	}
	// Help - Print commands
	else if(strcmp(line, "help") == 0)
	{
		printf(" - reset - resets c file\n");
		printf(" - clear - clears console\n");
		printf(" - print - prints variables and text\n");
		printf(" - exit - exits interpreter\n");
		printf(" - rline - removes the last line written\n");
		printf(" - help - shows commands\n");
		printf(" - show - displays code lines so far\n");
		return outc;
	}

	// Print function
	else if( isPrintCalled(line) )
	{
		char printCommand[MAX_LINE_SIZE];
		if( print_cmd(line, printCommand) )
		{
			writeln(outc, printCommand);
			compile(comp, args);
			(*lwritten)++;
			return outc;
		}
		return outc;
	}

	// Remove line (rline)
	// Maybe add an argument to remove N lines?
	else if(strcmp(line, "rline") == 0)
	{
		rline(outc, lwritten);
		return outc;
	}

	else if(strcmp(line, "show") == 0){
		printFileLines();
		return outc;
	}

	return NULL;
}


/* Interpreter loop */
void interpreter(char *compilerCall, char *args)
{
	//	CREATE C FILE
	FILE *outc;
	outc = fopen(CPATH, "w+");
	if(!outc){
		fprintf(stderr, " Error creating file\n");
		return;
	}
	/* Init main function */
	createMain(outc);

	/* Finish main */
	endMain(outc);
	fflush(outc);

	/* Return file pointer to middle of main*/
	seekMiddle(outc);
	fseek(outc, 0, SEEK_CUR);

	// INTERPRETER LOOP

	//Counter for total lines written by user
	int lwritten = 0;
	//Loop variable
	ushort loop = 1;
	while(loop == 1)
	{
		/* Get interpreter line */
		printf("\n");
		char line[100];
		char *ret = getstr(line, "> ");
		if(!ret || strcmp(ret, "\0") == 0)
			continue;
		/* Check commands */
		FILE *retf = checkCommands(line, outc, compilerCall, args, &lwritten);
		if(retf){
			outc = retf;
			continue;
		}
		/* Write interpreter line */
		writeln(outc, line);
		/* Compile & execute */
		compile(compilerCall, args);
		// Increase lines written counter
		lwritten++;
	}
	/* Close file */
	fclose(outc);
}



int main(int argc, const char **argv)
{
	char compilerCall[100] = "gcc";
	char arguments[100] = " ";

	arglib_init();
	arglib_add_option('c', "compiler", "system call for compiling C code", ARG_STR, ARG_OPT);
	arglib_add_option('a', "args", "input arguments to pass to compiled C code", ARG_STR, ARG_OPT);

	if(!arglib_parse(argc, argv)){
		return 1;
	}

	char *comp = arglib_get_value("compiler");
	if(comp && strlen(comp) < 100){
		strcpy(compilerCall, comp);
	}

	char *args = arglib_get_value("args");
	if(args && strlen(args) < 100){
		strcat(arguments, args);
	}

	arglib_free();
	

	//Just in case, delete temp files if exist
	delIfExist(CPATH);
	delIfExist(EXENAME);

	//Intro
	printf("\n");
	printf(" SCLI - Simple C Language Interpreter\n");
	printf(" %s (%s)\n", VERSION, VDATE);
	printf(" Type 'help' for command list\n");

	interpreter(compilerCall, arguments);

	return 0;
}