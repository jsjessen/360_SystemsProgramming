/* James Jessen             
 * 10918967                
 *
 * CptS 360
 */

#ifndef __UTIL_EXT2_H_
#define __UTIL_EXT2_H_


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ext2fs/ext2_fs.h>

typedef unsigned char  u8;
typedef unsigned short u16; 
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR; 

void  put_block(int dev, int block, u8* buf);
u8*   get_block(int dev, int block);
INODE get_inode(int dev, int inode_number);

SUPER* get_super       (int dev); 
void   put_super       (int dev, SUPER* buf); 

GD*    get_gd          (int dev);
void   put_gd          (int dev, GD* buf);

u8*    get_bmap(int dev);
void   put_bmap(int dev, u8* buf);

u8*    get_imap(int dev);
void   put_imap(int dev, u8* buf);

void set_free_inodes(int dev, int change);
void set_free_blocks(int dev, int change);
int ialloc(int dev);
int balloc(int dev);
int ifree (int dev, int inode); 
int bfree (int dev, int block); 

int get_magic       (int dev);
int get_block_size  (int dev);
int get_blocks_count(int dev);
int get_inodes_count(int dev);

int  test_bit (u8* buf, int bit);
void set_bit  (u8** buf, int bit);
void clear_bit(u8** buf, int bit);


#endif
