// James Jessen
// CptS 422
 
#ifndef __COMPARE_H__
#define __COMPARE_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "input.h"

// Compares 2 files line by line
// Prints significant lines that do not match
// Returns number of significant non-matching lines
// err_line1 is set to the first error line in file1
// err_line2 is set to the first error line in file2
int file_compare(char* filename1, char* filename2, int* err_line1, int* err_line2);

// Compares 2 lines
// Judging significance is beyond the scope of this function
// Returns number of significant differences
int line_compare(char* line_ptr1, char* line_ptr2);

// Returns true if line is significant
// Otherwise, returns false
int get_line_type(char* line_ptr);

#endif
