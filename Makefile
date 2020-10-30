
# Defines compiler to use
CC=gcc

# Defines compiler flats.
# '-I.' tells the compiler to search for header files on a directory.
# In this case, it searches the current directory '.'
CFLAGS=-I.

sclimake: scli.c arglib.c utils.c printcmd.c
	$(CC) -o scli scli.c arglib.c utils.c printcmd.c
