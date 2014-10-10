/* James Jessen
 * 10918967   
 *
 * CptS 360
 * Lab #4
 */

#ifndef __UTIL_H__
#define __UTIL_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Input an array of pointers like char* argv[]
// Goes through and frees each element
// and then frees the array
void free_array(char** array);

#endif
