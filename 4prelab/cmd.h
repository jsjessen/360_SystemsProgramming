/* James Jessen
 * 10918967
 *
 * CptS 360
 * Pre-work #4
 */

#ifndef __CMD_H__
#define __CMD_H__


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

#define BLK_SIZE 4096
#define BUF_SIZE 256
#define DIR_PERM 0775   // drwxrwxr-x
#define FILE_PERM 0664  // -rw-rw-r--

int my_mkdir(char* pathname);
int my_rmdir(char* pathname);
int my_rm   (char* pathname);
int my_cat  (char* pathname);
int my_cp   (char* source, char* target);
int my_ls   (char* pathname);

#endif
