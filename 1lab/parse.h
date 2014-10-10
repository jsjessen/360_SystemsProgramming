#ifndef __PARSE_H__
#define __PARSE_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parses the input to get the command and pathname.
int parse_input(char* input,    char** command, char** pathname);

// Parses the pathname to get the dirname and basename
int parse_path (char* pathname, char** dirname, char** basename); 


#endif
