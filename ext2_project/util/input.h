// James Jessen
// 10918967   
// 
// CptS 360

// ---------------------------------
// Functions dealing with user input
// ---------------------------------

#ifndef __INPUT_H__
#define __INPUT_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Gets input from user
// Returns a string with no leading whitespace
// and a single space between words
// Returns NULL if it receives no meaningful input
char* get_input();

// Example:
//          input  = "cmd   arg1  \t  arg2\n"
//          output = "cmd arg1 arg2"


// Parses input string by splitting on delimiters
// Returns an array of char* appended with a NULL entry
char** parse(const char* input, const char* delimiters);

// Example:
//          output[0] = "cmd"
//          output[1] = "arg1"
//          output[2] = "arg2"
//          output[3] =  NULL


// Parses a path in dirname and basename
int parse_path(const char* path, char** dirname, char** basename);

//  path       dirname   basename
//  -----------------------------
//  /usr/lib   /usr/     lib
//  /usr/      /         usr
//  usr        .         usr
//  /          /         /
//  .          .         .
//  ..         .         ..

#endif
