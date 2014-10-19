/* James Jessen             
 * 10918967                
 *
 * CptS 360
 */

#ifndef __PRINT_EXT2_H_
#define __PRINT_EXT2_H_


#include <stdio.h>
#include <stdlib.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <time.h>
#include <ext2fs/ext2_fs.h>

#include "util_ext2.h"

// Printing Bitmaps
#define GROUP_SIZE 8
#define GROUPS_PER_LINE 8

void print_super(int fd);
void print_gd   (int fd);
void print_inode(int fd, int inode);
void print_dir  (int fd);
void print_imap (int fd);
void print_bmap (int fd);


#endif
