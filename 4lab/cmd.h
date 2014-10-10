#ifndef __CMD_H__
#define __CMD_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// DO WE REALLY NEED ALL OF THESE?
// IF SO, WHAT FOR?
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX 1024 
#define DIR_PERM 0775   // drwxrwxr-x
#define FILE_PERM 0664  // -rw-rw-r--

char* (*get_cmd(char* cmd_name, char* caller))(char*); // Get function for "command"

char* my_help    (char* path); // Display a menu of the available commands
char* my_cat     (char* path); // Print the contents of a file
char* my_pwd     (char* path); // Print the current working directory
char* my_ls      (char* path); // List all files
char* my_cd      (char* path); // Change the current working directory
char* my_mkdir   (char* path); // Make a new directory
char* my_rmdir   (char* path); // Remove an existing directory
char* my_rm      (char* path); // Remove an existing file
char* my_quit    (char* path); // Quit

int send_file    (int socket, char* path, char* message); // Send file
int receive_file (int socket, char* path, int size);      // Recieve file

#endif
