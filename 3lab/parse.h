/* James Jessen
 * 10918967   
 *
 * CptS 360
 * Lab #3
 */

#ifndef __PARSE_H__
#define __PARSE_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parses input string by splitting on delimiters
// Returns an array of char* ending with NULL
char** parse(char* input, char* delimiters);

#endif
