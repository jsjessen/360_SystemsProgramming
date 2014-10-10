#include "input.h"

int get_input(char** input)
{
	int i = 0;
	int max = INITIAL_BUF_SIZE;

	char* buf = (char*)malloc(max); // allocate buffer
	if (buf == NULL)  
		return -1;

	buf[0] = 0;
	while (1) 
	{ 	
		int c = getchar();

		if (c == '\n')
		{
			if(buf[0] == 0)
				return -1;

			buf[i] = 0; // end string with null char
			break;
		}

		// Replace block of whitespace with single space
		if (isspace(c)) 
		{
			if(buf[0] == 0)
				continue;

			while(isspace(c)) {	c = getchar(); }
			ungetc(c, stdin);

			buf[i++] = ' ';
			continue;
		}

		buf[i] = c;

		if (i == (max - 1)) 
		{ // buffer is full
			max = max + max;

			buf = (char*)realloc(buf, max); // double size of buffer 
			if (buf == NULL)  
				return -1;
		}

		i++; // next char
	}

	*input = buf;
	return 0;
}
