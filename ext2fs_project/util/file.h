
// James Jessen
// 10918967   
// 
// CptS 360

// ------------------------------
// ------------------------------

#ifndef __FILE_H_
#define __FILE_H_


#include <stdio.h>

#include <global.h>
#include <transfer.h>
#include <search.h>
#include <util.h>


int open_file(const char* pathname, int mode);
int close_file(int fd);

int read_file(int fd, void* buf, int nbyte);
int write_file(int fd, void* buf, int nbyte);

OPEN_FILE* get_file(int fd);

int seek_file(int fd, int position);
int truncate_file(MINODE *mip);

int dup(int fd);
int dup2(int fd, int gd);


#endif
