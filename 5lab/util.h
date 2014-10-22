// James Jessen
// 10918967   
// 
// CptS 360

// ------------------------------------
// Miscellaneous utility functions for
// interacting with an  ext2 filesystem
// ------------------------------------

#ifndef __UTIL_H_
#define __UTIL_H_


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ext2fs/ext2_fs.h>

typedef unsigned char  u8;
typedef unsigned short u16; 
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef enum { false, true } bool;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR; 

int  test_bit (u8*  buf, int bit);
void set_bit  (u8** buf, int bit);
void clear_bit(u8** buf, int bit);


#endif
