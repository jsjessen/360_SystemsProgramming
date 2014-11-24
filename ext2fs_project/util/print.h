// James Jessen
// 10918967   
// 
// CptS 360

// --------------------------------------
// Functions for printing aspects of the
// ext2 filesystem for the user's viewing
// --------------------------------------

// Common Abbreviations
// --------------------
// fs   = filesystem
// fd   = file descriptor
// imap = inode_bitmap
// bmap = block_bitmap

#ifndef __PRINT_H_
#define __PRINT_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ext2fs/ext2_fs.h>
#include <sys/stat.h>

#include <global.h>
#include <util.h>
#include <transfer.h>

// Printing Bitmaps
#define GROUP_SIZE 8
#define GROUPS_PER_LINE 10 

// Printing title/divider
#define TITLE_WIDTH 50

void print_title  (char* title, char symbol);
void print_divider(char symbol);

// Should they accept device or a pointer to super, gd, inode, etc... ?
void print_super(int dev);
void print_gd   (int dev);
void print_inode(int dev, int inode_number);
void print_dir  (int dev, int inode_number);
void print_imap (int dev);
void print_bmap (int dev);

void print_file_blocks(int device, int inode_number);
int  print_indirect_block(int device, int block_size, int* buf, int level);


#endif
