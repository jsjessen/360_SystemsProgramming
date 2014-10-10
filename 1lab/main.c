#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "input.h"
#include "parse.h"
#include "cmd.h"

#define DEBUG

static char* input;		// string the user enters 
static char* command;	// 1st word from input
static char* pathname;	// 2nd word from input
static char* dirname;	// a/b/c from pathname a/b/c/d
static char* basename;	// d     from pathname a/b/c/d

int main()
{
	if(initialize() != 0)
	{
		fprintf(stderr, "Initialization failed!\n");
		exit(1);
	}
	else
		printf("\nRoot Initialized\n");

	printf("Enter ? for help menu");

	while(1)
	{
		int (*fptr)(char*, char*);	// Pointer to function associated with command

		// Release the memory that was allocated for the strings
		free(input);		input    = NULL;
		free(command);		command  = NULL;
		free(pathname);		pathname = NULL;
		free(dirname);		dirname  = NULL;
		free(basename);		basename = NULL;

		// Prompt the user to enter a command
		printf("\nCommand: ");

		// Get input from the user
		if(get_input(&input) != 0)
		{
			if(input != NULL)
				fprintf(stderr, "Invalid Input\n");

			continue;
		}
		else
			printf("Input = \"%s\"\n", input);

		// Parse the input to get the command and pathname
		if(parse_input(input, &command, &pathname) != 0)
		{
			fprintf(stderr, "Failed to parse the input\n");
			continue;
		}
		else
			printf("Command = \"%s\"\nPathname = \"%s\"\n", command, pathname);

		// Given the command name, get a pointer to that command's function
		if((fptr = get_cmd(command)) == NULL)
		{
			fprintf(stderr, "Invalid Command\n");
			continue;
		}
		else
			printf("Command Found\n");

		// Parse the pathname to get the dirname and basename
		if(parse_path(pathname, &dirname, &basename) != 0)
		{
			fprintf(stderr, "Failed to parse the path\n");
			continue;
		}
		else
			printf("dirname = \"%s\"\nbasename = \"%s\"\n", dirname, basename);

		// Execute the command, providing the dirname and basename
		if(fptr(dirname, basename) != 0)
		{
			fprintf(stderr, "%s: Failed\n", command);
			continue;
		}
		else
			printf("%s: Success!", command);

		putchar('\n');
	}
}
