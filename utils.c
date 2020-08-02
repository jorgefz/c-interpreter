#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/*
Removes every instance of a character from a string
*/
char *
rmch(char *str, char rm)
{
	char temp[strlen(str)];
	int c = 0;
	for(size_t i=0; i<strlen(str); i++)
	{
		if(str[i] == rm)
			continue;
		temp[c] = str[i];
		c++;
	}
	temp[c] = '\0';
	strcpy(str, temp);
	return str;
}


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


/*
Given a string and a delimiter,
it returns the number of tokens in the string.
*/
size_t
strtokn(char *str, const char delim)
{
	if(!str)
		return 0;
	size_t n = 0;
	for(size_t i=0; i<strlen(str); i++){
		if(str[i] == delim)
			n++;
	}
	return (n+1);
}


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
strpar(char *ptr, const char delim)
{
	if(!ptr)
		return (NULL);
 
	while(1)
	{
		if(ptr[0] == delim){
			ptr[0] = '\0';
			ptr++;
			break;
		}

		else if(ptr[0] == '\0')
			return (NULL);
		ptr++;
	}

	return ptr;
}

/*
Given a string 'str' and a delimiter 'delim',
it splits the string at the delimiters, stores
the pointers to the tokens in 'dest', and returns
a pointer to it.
On fail, it returns NULL.
*/
char **
strsplit(char **dest, char *str, const char delim)
{
	if(!str)
		return (NULL);

	char *ptr = str;
	size_t c = 0;

	while( ptr != NULL ){
		dest[c] = ptr;
		c++;
		ptr = strpar(ptr, delim);
	}

	return dest;
}