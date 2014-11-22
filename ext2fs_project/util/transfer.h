// James Jessen
// 10918967   
// 
// CptS 360

// ------------------------------
// Functions for transfering data
// to and from an ext2 filesystem
// ------------------------------

#ifndef __TRANSFER_H_
#define __TRANSFER_H_


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ext2fs/ext2_fs.h>

#include <global.h>

u8*     get_block(int device, int block);
SUPER*  get_super(int device); 
GD*     get_gd   (int device);
u8*     get_bmap (int device);
u8*     get_imap (int device);
INODE*  get_inode(int device, int inode_number);
MINODE* iget     (int device, int inode_number);

void put_block(int device, int block, u8* buf);
void put_super(int device, SUPER* buf); 
void put_gd   (int device, GD* buf);
void put_bmap (int device, u8* buf);
void put_imap (int device, u8* buf);
void put_inode(int device, int inode_number, INODE inode);
void iput     (MINODE *mip);

int get_magic            (int device);
int get_block_size       (int device);
int get_inode_size       (int devive);
int get_blocks_count     (int device);
int get_inodes_count     (int device);
int get_inodes_per_block (int device);
int get_inode_table_block(int device);

bool isExt2(int device);


#endif
