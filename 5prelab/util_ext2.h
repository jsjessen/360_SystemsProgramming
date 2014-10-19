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

u8*   get_block(int fd, int block);
INODE get_inode(int fd, int inode_number);

SUPER*  get_super       (int fd); 
GD*     get_gd          (int fd);
u8*     get_block_bitmap(int fd);
u8*     get_inode_bitmap(int fd);

int ialloc(int fd, int inode); // Allocates a free Inode, returns inode #
int balloc(int fd, int inode); // Allocates a free Block, returns block #

int get_magic       (int fd);
int get_block_size  (int fd);
int get_blocks_count(int fd);
int get_inodes_count(int fd);

int  test_bit (u8* buf, int bit);
void set_bit  (u8* buf, int bit);
void clear_bit(u8* buf, int bit);


#endif
