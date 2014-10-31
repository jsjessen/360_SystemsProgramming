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

#include "type.h"

int  test_bit (u8*  buf, int bit);
void set_bit  (u8** buf, int bit);
void clear_bit(u8** buf, int bit);

#endif
