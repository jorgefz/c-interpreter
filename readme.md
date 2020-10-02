# SCLI - Simple C Language Interpreter

Line per line interactive C compiler wrapper. This program provides a console on which input lines are interpreted as C code. The line is first written to a temporary C file, which is then compiled to a Windows executable and executed.

The console can also recognise a series of special commands to interact with the C file and facilitate code writing.

## Compilation

To compile the code, input the files `scli.c`, `printcmd.c`, and `utils.c` into your compiler arguments. For example,

```
gcc -Wall -Wextra scli.c printcmd.c utils.c arglib.c -o scli
```

## Command Line Arguments

The program comes with simple command line arguments:

* '-h' '--help': Displays available commands
* '-c' '--compiler': system call for compiling C code in your system, enclosed by double quotes.
* '-a' '--args': arguments to include when calling the compiled C code.

For example,

```
scli --compiler "gcc -Wall -Wextra" --args "-o output.dat --flag"
```

## Commands

* 'help' - displays the command list
* 'clear' - clears the console
* 'reset' - resets the C file to the default empty main() function.
* 'exit' - exits the console and deletes the C file
* 'print' - simplifies printf function (described below)
* 'rline' - deletes the last C code line written (WIP)

## Print command

The print command allows to quickly print text and variable values instead of typing the full printf syntax. The command is called in the following way:

` print [arguments]`

There are two types of arguments, which can be input in any order. Text arguments are directly copied into the printf function, and should be enclosed in double quotes " ".

` print "This is a text argument" `

```c
/* Output */
printf("This is a text argument\n");
```

The second type of argument are variables. These are called by typing the data type of the variable along with the variable name, separated by a whitespace.

` print int num `

```c
/* Output */
printf("%d\n", num);
```

The list of supported data types and their corresponding format specifiers are:

| Data type name 	| Format specifier |
|:-----------------:|:----------------:|
| int 				| `%d`			   |
| float 			| `%f`			   |
| double 			| `%g`			   |
| char 				| `%c`			   |
| string 			| `%s`			   |

The arguments above can be combined in any order.

` print int a "times" int b "is equal to" int a*b `
```c
printf("%d times %d is equal to %d\n", a, b, a*b);
```

` print "I'm sorry," string name ". I'm afraid I can't do that." `
```c
printf("I'm sorry, %s. I'm afraid I can't do that \n", name);
```

` print "The character" char c "has integer value" int c `
```c
printf("The character %c has integer value %d\n", c, c);
```

It also works for doing fast calculations.

` print "Mass of Jupiter in energy:" double 1.9E27*pow(3E8,2) "Joules" `

Output: `Mass of Jupiter in energy:   1.71e+044 Joules`