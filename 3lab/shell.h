/* James Jessen
 * 10918967   
 *
 * CptS 360
 * Lab #3
 */

#ifndef __SHELL_H__
#define __SHELL_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "parse.h"

#define FILE_PERM 0644

// Searches through env for env[i] = "TARGET=..."
// Returns char* pointing at --------------- ^
char* search_env(char* env[], char* target);

// Input an array of pointers like char* argv[]
// Goes through and frees each element
// and then frees the array
void free_array(char** array);

// Changes the current working directory
// to arg1 if provided, otherwise to $HOME
// Returns 0 on success and -1 on failure
int my_cd(char* arg1, char* home);

// Searches through my_argv for <, >, >>
// Assumes the following element is a file
// Redirects IO accordingly
void io_redirect(char** my_argv);

// Processes a line of input
void process_line(char* line, char** paths, char* env[]);

// Attempts to execute line
// Searches for command in each PATH dir
void do_command(char* line, char** paths, char* env[]);

#endif
