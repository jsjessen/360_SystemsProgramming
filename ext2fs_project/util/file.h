
// James Jessen
// 10918967   
// 
// CptS 360

// ------------------------------
// Functions for transfering data
// to and from an ext2 filesystem
// ------------------------------

#ifndef __FILE_H_
#define __FILE_H_


#include <stdio.h>

#include <global.h>
#include <transfer.h>
#include <search.h>


int open_file(char* pathname, int mode);
int close_file(int fd);

int my_read(int fd, char* buf, int nbytes);

OPEN_FILE* get_file(int fd);

int my_lseek(int fd, int position);
int my_truncate(MINODE *mip);

int dup(int fd);
int dup2(int fd, int gd);


#endif
