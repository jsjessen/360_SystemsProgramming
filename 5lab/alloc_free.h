// James Jessen
// 10918967   
// 
// CptS 360

// ------------------------------------
// Functions for allocating and freeing
// memory primarialy an ext2 filesystem
// ------------------------------------

#ifndef __ALLOC_FREE_H_
#define __ALLOC_FREE_H_


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ext2fs/ext2_fs.h>

void set_free_inodes(int device, int change);
void set_free_blocks(int device, int change);
int ialloc(int device);
int balloc(int device);
int ifree (int device, int inode); 
int bfree (int device, int block); 


#endif
