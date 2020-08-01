

/*
	==================  SCLI ===================
		=== Simple C Language Interpreter ===

	HISTORY
	Version 1.0 - 30/07/2020
		- Added main interpreter
		- Added commands:
			reset, exit, clear, help

	TO-DO
		- Command line arguments:
			> Choose compiler & options (gcc, clang, etc)
			> Choose output filename 
		- Support for other systems' command calls
		- Command to navigate file pointer above main
			to write includes, macros, structs, and functions
		-Print command for quick printf

		- Get whether compilation failed with
			return value of system call.
			SUCCESS: ret = 0
			FAILED: ret = 1

*/

const char *VERSION = "V 1.0";
const char *VDATE = "30/07/2020";


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short ushort;


/* Slices string at two points */
char *
strslc(char *s, size_t i, size_t j)
{
	/* Input checks */
	if( (i>j) || strlen(s)<(j-i+1) )
		return (NULL);

	/* Move pointer to left slice */
	char *ptr;
	ptr = s + i;

	/* Null terminator after right slice */
	ptr[j-i+1] = '\0';
	
	s = ptr;
	return s;
}


/* Gets an input string from user */
char *getstr(char *dest, const char *msg)
{
	size_t max_read = (size_t) 1E5;
	printf("%s", msg);
	char *ptr = fgets(dest, max_read, stdin);
	if(!ptr)
		return (NULL);
	// Removes newline character from end of string
	if (strchr(dest, '\n'))
		strslc(dest, 0, strlen(dest)-2);
	return dest;
}



/* Create beginning of basic main program */
void createMain(FILE *outc)
{
	//Include standard libraries
	fprintf(outc, "#include <stdio.h>\n");
	fprintf(outc, "#include <stdlib.h>\n\n");
	// Initiate main function
	fprintf(outc, "int main(void){\n\n");
}

/* Moves stream pointer to the line before 'return 0;' */
void seekMiddle(FILE *outc)
{
	//Return just before ending statements of main
	int move = sizeof("return 0;\n}\n") + 1;
	fseek(outc, -move, SEEK_END);
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
	fprintf(outc, "%s\n", line);
	endMain(outc);
	seekMiddle(outc);
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
void compile(char *compilerCall)
{
	char call[strlen(compilerCall)+20];
	strcpy(call, compilerCall);
	strcat(call, " outc.c -o outc");
	if(checkFile("outc.c") == 1)
	{
		int r = system(call);
		printf(" Return value is %d\n", r);
	}
	if(checkFile("outc.exe") == 1)
		system("outc");
}


void delIfExist(const char *path)
{
	if(checkFile(path) == 1)
	{
		char del[strlen(path)+5];
		strcpy(del, "del ");
		strcat(del, path);
		system(del);
	}
}

/*
Checks if input line is command
Returns file pointer if sucessfull
and NULL otherwise
*/
FILE *checkCommands(const char *line, FILE *outc)
{
	// Exit program
	if(strcmp(line, "exit") == 0){
		fclose(outc);
		delIfExist("outc.c");
		delIfExist("outc.exe");
		exit(0);
	}
	
	// Reset c file
	else if(strcmp(line, "reset") == 0)
	{
		fclose(outc);
		//Reset contents with new instance
		FILE *temp = fopen("outc.c", "w");
		fclose(temp);
		//Redo file
		FILE *new = fopen("outc.c", "w+");
		createMain(new);
		endMain(new);
		seekMiddle(new);
		//Delete executable
		delIfExist("outc.exe");
		//Clear
		checkCommands("clear", outc);
		return new;
	}
	// Clear
	else if(strcmp(line, "clear") == 0)
	{
		for(int i=0; i<20; i++)
			printf("\n");
		return outc;
	}
	// Print commands
	else if(strcmp(line, "help") == 0)
	{
		printf(" - reset - resets c file\n");
		printf(" - clear - clears console\n");
		printf(" - exit - exits interpreter\n");
		printf(" - help - shows commands\n");
		//printf(" - resetline - resets last written line (WIP)\n");
		return outc;
	}
	return NULL;
}


/* Interpreter loop */
void interpreter(char *compilerCall)
{
	//	CREATE C FILE
	FILE *outc;
	outc = fopen("outc.c", "w+");
	if(!outc){
		fprintf(stderr, " Error creating file\n");
		return;
	}
	/* Init main function */
	createMain(outc);
	/* Finish main */
	endMain(outc);
	/* Return file pointer to middle of main*/
	seekMiddle(outc);

	// INTERPRETER LOOP
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
		FILE *retf = checkCommands(line, outc);
		if(retf){
			outc = retf;
			continue;
		}
		/* Write interpreter line */
		writeln(outc, line);
		/* Compile & execute */
		compile(compilerCall);
	}
	/* Close file */
	fclose(outc);
}



int main(int argc, char **argv)
{
	char compilerCall[100] = "gcc";

	//Input commands
	if(argc == 2){
		if( strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0 ){
			printf(" Commands:\n");
			printf(" -h, --help: displays available commands\n");
			printf(" -c, --compiler: between double quotes;");
			printf(" system call for compiling C code in your machine\n");
			printf("	-> scli -c \"gcc -Wall -Wextra\" ");
		}
		else{
			printf(" Unknown command\n");
			return 0;
		}
	}

	else if( argc == 3 ){
		if ( strcmp(argv[1],"-c") == 0 || strcmp(argv[1],"--compiler") == 0 )
			strcpy(compilerCall, argv[2]);
		else{
			printf(" Unknown command\n");
			return 0;
		}
	}
	
	else if( argc > 3){
		printf(" Too many commands\n");
		return 0;
	}
	

	//Just in case, delete temp files if exist
	delIfExist("outc.c");
	delIfExist("outc.exe");

	//Intro
	printf("\n");
	printf(" SCLI - Simple C Language Interpreter\n");
	printf(" %s (%s)\n", VERSION, VDATE);
	printf(" Type 'help' for command list\n");

	interpreter(compilerCall);

	return 0;
}