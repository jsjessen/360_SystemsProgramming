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

#include <global.h>
#include <transfer.h>
#include <memory.h>

int rm_child(MINODE *parent_mip, int child_ino);

DIR* get_last_dir_entry(u8* block_start, int block_size);

bool isEmptyDir(MINODE *mip);

int get_ideal_record_length(const int name_len);

int enter_name(MINODE *parent_mip, int my_ino, char *my_name);


#endif
