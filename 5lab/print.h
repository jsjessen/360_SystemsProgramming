/* James Jessen             
 * 10918967                
 *
 * CptS 360
 */

#ifndef __PRINT_EXT2_H_
#define __PRINT_EXT2_H_


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ext2fs/ext2_fs.h>

#include "util_ext2.h"

// Printing Bitmaps
#define GROUP_SIZE 8
#define GROUPS_PER_LINE 8

void print_super(int dev);
void print_gd   (int dev);
void print_inode(int dev, int inode_number);
void print_dir  (int dev, int inode_number);
void print_imap (int dev);
void print_bmap (int dev);


#endif
