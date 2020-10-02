


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arglib.h"


/* Data type string names corresponding to data type macro values */
static char *_dtypes[] = {"flag", "int", "float", "string"};

/* String representation for required/optional flags */
static char *_reqflag[] = {"(required)", "(optional)"};

/* Length of argument list, global */
size_t ARG_NUM = 0;


/*
Returns the current number of predefined arguments
*/
int arglib_argnum()
{
	return ARG_NUM;
}

/*
Adds a new argument to the argument list with the input parameters.
If the parameter values are out of bounds, NULL is returned.
*/
_ARGS *arglib_add_option(_ARGS *args,
							const char c_label,
							const char *s_label, 
							const char *descr,
							int type, int req)
{
	
	// Check if input parameters are out of bounds
	if(s_label && strlen(s_label)+1 > (size_t)ARG_MAX_SLABEL_LEN)
		return NULL;

	if(descr && strlen(descr)+1 > (size_t)ARG_MAX_DESC_LEN)
		return NULL;

	if(type < ARG_FLAG || type > ARG_STR)
		return NULL;

	if(req != ARG_REQ && req != ARG_OPT)
		return NULL;
	
	//Check that labels are unique
	for(size_t i=0; i<ARG_NUM; i++)
	{
		if(s_label && strcmp(s_label,args[i].s_label)==0){
			return NULL;
		}
		if(c_label==args[i].c_label){
			return NULL;
		}
	}

	// Increase list size
	ARG_NUM++;
	args = realloc(args, sizeof(_ARGS) * ARG_NUM );

	// Construct new argument element
	_ARGS new_arg = {.c_label = c_label, .dtype = type, .flag_req = req};
	
	if(s_label){ strcpy(new_arg.s_label, s_label);}
	else{ strcpy(new_arg.s_label, "\0");}

	if(descr){ strcpy(new_arg.descr, descr);}
	else{ strcpy(new_arg.descr, "\0");}

	// Insert into argument list
	memcpy(args+(ARG_NUM-1), &new_arg, sizeof(_ARGS));

	return args;
}

/*
Initialises option list and adds the help flag
*/
_ARGS *arglib_init()
{
	_ARGS *args = NULL;
	ARG_NUM = 0;

	//Initialise with first command: help
	args = arglib_add_option(args, 'h', "help", "Displays valid arguments and exits the program", ARG_FLAG, ARG_OPT);

	return args;
}

/*
Frees option argument list
*/
void arglib_free(_ARGS *args)
{
	for(size_t i=0; i<ARG_NUM; i++){
		free(args[i].pval);
	}
	free(args);
	ARG_NUM = 0;
}

/*
Displays information of all predefined options.
*/
static void arglib_help(_ARGS *args, const char *argv[])
{
	printf(" USAGE: %s --[LABEL] [VALUE] --[FLAG]\n", argv[0]);
	printf("        %s -[LABEL] [VALUE] -[FLAG]\n", argv[0]);

	for(size_t i=0; i<ARG_NUM; i++)
	{
		// Char Label
		printf(" %5s-%c,", "", args[i].c_label);
		// Multi-char label
		printf(" %5s--%s ", "", args[i].s_label);

		// Param type & Req
		printf(": %10s(%s)%s", "", _dtypes[args[i].dtype], _reqflag[args[i].flag_req]);

		// Description
		printf(" %s\n", args[i].descr);
	}
}


/*
Displays usage of an input option argument
*/
static void arglib_usage_info(_ARGS *arg)
{
	printf(" Usage: ");
	// Char Label
	printf(" %5s-%c,", "", arg->c_label);
	// Multi-char label
	printf(" %2s--%s ", "", arg->s_label);
	// Parameter data type
	if(arg->dtype != ARG_FLAG){
		printf(" [%s]", _dtypes[arg->dtype]);
	}
	// Description
	printf(" %s\n", arg->descr);
}




/*
Converts a input string 'str' into an integer.
The result is stored at the input pointer 'dest'.
On fail, NULL is returned.
*/
static int *arglib_strtoint(int *dest, const char *str)
{
	char *endptr;
	int val;

	//Convert string to integer.
	val = (int) strtol(str, &endptr, 10);
	//Checks if conversion worked
	if(endptr == str){
		return (NULL);
	}
	*dest = val;
	return dest;
}


/*
Convert string to double.
*/
static double *arglib_strtoflt(double *dest, const char *str)
{
	char *endptr;
	double val;
	// Converts string to double
	val = (double) strtod(str, &endptr);
	// Checks if conversion worked
	if(endptr == str){
		return (NULL);
	}
	*dest = val;
	return dest;
}


/*
Compares input argument with a predefined option label.

*/
static _ARGS *arglib_cmp_arg(_ARGS *arg, const char *argv)
{
	// Single-char argument
	if(strlen(argv)==2 && argv[0] == '-' && argv[1] != '-')
	{
		if(arg->c_label == argv[1]){
			return arg;
		}
		return NULL;
	}
	// Multi-char argument
	if(strlen(argv)>2 && argv[0] == '-' && argv[1] == '-')
	{
		if( strcmp(argv+2, arg->s_label) == 0 ){
			return arg;
		}
	}
	return NULL;
}


/*
Casts an input string parameter into a predefined argument's data type,
and stores it in memory.
*/
static _ARGS *arglib_cast_arg(_ARGS *arg, const char *argv)
{
	// Integer type
	if(arg->dtype == ARG_INT){
		int idata;
		if(arglib_strtoint(&idata, argv) == NULL){
			return NULL;
		}
		arg->pval = malloc(sizeof(int));
		*((int*)arg->pval) = idata;
		return arg;
	}
	// Float type
	if(arg->dtype == ARG_FLOAT){
		double fdata;
		if(arglib_strtoflt(&fdata, argv) == NULL){
			return NULL;
		}
		arg->pval = malloc(sizeof(double));
		*((double*)arg->pval) = fdata;
		return arg;
	}
	// String type
	if(arg->dtype == ARG_STR){
		arg->pval = malloc(strlen(argv)+1);
		strcpy(arg->pval, argv);
		return arg;
	}
	return NULL;
}


/*
Compares input arguments with defined argument list
and extracts values. 
*/
_ARGS *arglib_parse(_ARGS *args, const int argc, const char *argv[])
{

	// Look for HELP command
	for(size_t i=1; i<(size_t)argc; i++)
	{
		if( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 ){
			arglib_help(args, argv);
			return NULL;
		}
	}
	// NOTE: consider looping by (1st) defined options
	// and then input args.
	// That way, help comnand is read first automatically

	/*
	Loop over input arguments.
	Ignore the first argument (program call name)
	by beginning the loop at i=1
	*/
	for(size_t i=1; i<(size_t)argc; i++)
	{
		//Find corresponding defined arg
		short found = 0;
		for(size_t j=1; j<ARG_NUM; j++)
		{
			if(arglib_cmp_arg(&args[j], argv[i])){
				found = 1;
				//Check if value already read
				if(args[j].pval){
					fprintf(stderr, " [ARGLIB] Error: argument '%s' already defined\n", argv[i]);
					return NULL;
				}
				//Seek parameter for non-flag arg
				if(args[j].dtype != ARG_FLAG){
					//Catch parameter following label
					if(i == (size_t)argc-1 || arglib_cast_arg(&args[j],argv[i+1]) == NULL){	
						fprintf(stderr, " [ARGLIB] Error: argument '%s' requires %s parameter\n", argv[i], _dtypes[args[j].dtype]);
						arglib_usage_info(&args[j]);
						return NULL;
					}
					i++;
				}
				//Set defined flag value as '0'
				else{
					args[j].pval = malloc(sizeof(int));
					*((int*)args[j].pval) = 0;
				}
				break;
			}
		}
		if(found == 0){
			fprintf(stderr, " [ARGLIB] Error: unknown argument '%s'\n", argv[i]);
			exit(-1);
		}
	}

	//Check that all required arguments are filled
	for(size_t i=0; i<ARG_NUM; i++)
	{
		if(args[i].flag_req == ARG_REQ && !args[i].pval){
			fprintf(stderr, " [ARGLIB] Error: required argument '%s' not defined\n", args[i].s_label);
			return NULL;
		}
	}


	return args;
}



/*
Retrieves the input argument value read from the parser.
The value must be cast to te appropriate data type.
A NULL value means no value was read.
*/
void *arglib_get_value(_ARGS *args, const char *arg_label)
{
	for(size_t i=0; i<ARG_NUM; i++)
	{
		if( strlen(arg_label) > 1 && strcmp(args[i].s_label, arg_label) == 0)
			return (args[i].pval);
		if( strlen(arg_label) == 1 && args[i].c_label == arg_label[0])
			return (args[i].pval);
	}
	return NULL;
}



