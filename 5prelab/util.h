/* James Jessen             
 * 10918967                
 *
 * CptS 360
 * PreLab #5
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <ext2fs/ext2_fs.h>

#define BLK_SIZE 1024

int MAGIC;
int BLOCKS_COUNT;
int INODES_COUNT;
int BLOCK_BITMAP;
int INODE_BITMAP;
int INODE_TABLE;

typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR; 

int get_block(int fd, int block, char *buf);

int gd   (int fd);
int imap (int fd);
int bmap (int fd);
int dir  (int fd);
int inode(int fd);
int super(int fd);
