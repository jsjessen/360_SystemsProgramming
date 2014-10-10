#include "parse.h" 

int parse_input(char* input, char** command, char** pathname)
{
	char* cptr;

	cptr = strtok(input, " ");
	if(cptr == NULL) // must have a command
		return -1;

	*command = (char*)malloc(strlen(cptr) + 1); //strlen does not include null char
	if(*command == NULL) 
		return -1; // memory allocation failed

	strcpy(*command, cptr);


	cptr = strtok(NULL, " ");
	if(cptr == NULL) // do not need pathname for commands like quit and help 
	{
		fprintf(stderr, "Warning: No pathname\n");
		return 0;
	}

	*pathname = (char*)malloc(strlen(cptr) + 1); //strlen does not include null char
	if(*pathname == NULL) 
		return -1; // memory allocation failed

	strcpy(*pathname, cptr);

	return 0;
}

int parse_path(char* pathname, char** dirname, char** basename) 
{
	int i;
	char* cptr;

	if(pathname == NULL)
		return 0;

	// strrchr finds the last occurence of a character
	cptr = strrchr(pathname, '/');
	if(cptr && cptr != pathname && cptr[1] == 0) // remove extraneous '/' at end
		*cptr = 0;

	i = strlen(pathname);
	while(i >= 0 && pathname[i] != '/') { i--; }
	i++; // last '/' is the i-th char in pathname

	if(i != 0)
	{
		*dirname = (char*)malloc(i + 1); // 1 for / and 1 for null char
		if(*dirname == NULL)
			return -1; // memory allocation failed

		strncpy(*dirname, pathname, i);
		(*dirname)[i] = 0; 
	}

	if(i == 1 && strlen(pathname) ==  1)
		return 0;

	*basename = (char*)malloc(strlen(pathname + i) + 1);
	if(*basename == NULL)
		return -1; // memory allocation failed

	strcpy(*basename, pathname + i);

	return 0;
}
