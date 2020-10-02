

#ifndef _ARGLIB_H_
#define _ARGLIB_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Argument data type macros */
#define ARG_FLAG 0
#define ARG_INT 1
#define ARG_FLOAT 2
#define ARG_STR 3

/* Argument required/optional flag macros */
#define ARG_REQ 0
#define ARG_OPT 1

/* MAX string length of multi-char label */
#define ARG_MAX_SLABEL_LEN 16

/* MAX string length of arg description */
#define ARG_MAX_DESC_LEN 50


typedef struct args_struct{
	// Parameter single character label (e.g. '-o')
	char c_label;
	// Parameter long label (e.g. '--output')
	char s_label[ARG_MAX_SLABEL_LEN];
	// Parameter description shown in help command
	char descr[ARG_MAX_DESC_LEN];
	// Data types of parameter
	int dtype;
	// Required/Optional flag for parameter
	int flag_req;
	// Parameter value
	void *pval;
} _ARGS;


/*
Returns the current number of predefined arguments
*/
int arglib_argnum();


/*
Adds a new argument to the argument list with the input parameters.
If the parameter values are out of bounds, NULL is returned.
*/
_ARGS *arglib_add_option(_ARGS *args,
						const char c_label,
						const char *s_label, 
						const char *descr,
						int type, int req);

/*
Initialises argument list and adds the help argument
*/
_ARGS *arglib_init();


/*
Frees option argument list
*/
void arglib_free(_ARGS *args);

/*
Interprets the command line arguments
and reads options and parameters.
*/
_ARGS *arglib_parse(_ARGS *args, const int argc, const char *argv[]);

/*
Retrieves the value of an option read from the input arguments.
The value must be cast to the appropriate data type.
Undefined options will return NULL.
Flags will return (int)0 if defined, and NULL otherwise. 
*/
void *arglib_get_value(_ARGS *args, const char *arg_label);

#endif



