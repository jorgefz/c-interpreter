

#ifndef _UTILS_H
#define _UTILS_H 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Removes every instance of a character from a string
*/
char *
rmch(char *str, char rm);


char *
strslc(char *s, size_t i, size_t j);

/* Gets an input string from user */
char *getstr(char *dest, const char *msg);


/*
Given a string and a delimiter,
it returns the number of tokens in the string.
*/
size_t
strtokn(char *str, const char delim);


/*
Given a string and a delimiter,
it runs through the first field
of the string until the delimiter,
substitutes the delimiter by a "\0",
and returns a pointer to the next field.
If the end of the string is reached, a NULL
pointer is returned.
*/
char *
strpar(char *ptr, const char delim);

/*
Given a string 'str' and a delimiter 'delim',
it splits the string at the delimiters, stores
the pointers to the tokens in 'dest', and returns
a pointer to it.
On fail, it returns NULL.
*/
char **
strsplit(char **dest, char *str, const char delim);



#endif