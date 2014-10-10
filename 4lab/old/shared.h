//********** shared.c **********

/* James Jessen             Group #1
 * 10918967                  Seq. #4
 *
 * Austin McShane           Group #2
 * 11350053                  Seq. #29
 *
 * CptS 360
 * Lab #4
 */

#ifndef __SHARED_H__
#define __SHARED_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX 256
#define DIR_PERM 0775   // drwxrwxr-x
#define FILE_PERM 0664  // -rw-rw-r--

char* my_cat(char* pathname);
char* my_pwd(char* pathname);
char* my_ls(char* pathname);
char* my_cd(char* pathname);
char* my_mkdir(char* pathname);
char* my_rmdir(char* pathname);
char* my_rm(char* pathname);


#endif //__SHARED_H__
