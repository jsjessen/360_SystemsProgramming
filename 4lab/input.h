/* James Jessen
 * 10918967   
 *
 * CptS 360
 * Lab #3
 */

#ifndef __INPUT_H__
#define __INPUT_H__


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "parse.h"

#define INITIAL_BUF_SIZE 8

// Gets input from user
// Returns a string with no leading whitespace
// and a single space between words
// Returns NULL upon failure
char* get_input();

// Example:
// input  = "cmd   arg1  \t  arg2\n"
// output = "cmd arg1 arg2"

#endif
