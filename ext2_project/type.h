/* type.h file for CS360 FS */

#include <stdio.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>   // MAY NEED "ext2_fs.h"
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

#define BLOCK_SIZE     1024

// Block number of EXT2 FS on FD
#define SUPERBLOCK        1
#define GDBLOCK           2
#define ROOT_INODE        2

// Default dir and regulsr file modes
#define DIR_MODE    0040777 
#define FILE_MODE   0100644
#define SUPER_MAGIC  0xEF53
#define SUPER_USER        0

// Proc status
#define FREE              0
#define READY             1
#define RUNNING           2

// Table sizes
#define NMINODES        100
#define NMOUNT           10
#define NPROC            10
#define NFD              10
#define NOFT            100

// Open File Table
typedef struct oft{
  int   mode;
  int   refCount;
  struct minode *inodeptr;
  int   offset;
}OFT;

// PROC structure
typedef struct proc{
  int   uid;
  int   pid, gid;
  int   status;
  struct minode *cwd;
  OFT   *fd[NFD];
}PROC;
      
// In-memory inodes structure
typedef struct minode{		
  INODE INODE;               // disk inode
  int   dev, ino;
  int   refCount;
  int   dirty;
  int   mounted;
  struct mount *mountptr;
}MINODE;

// Mount Table structure
typedef struct mount{
        int    dev;
        int    nblocks,ninodes;
        int    bmap, imap, iblk;
        MINODE *mounted_inode;
        char   name[64]; 
        char   mount_name[64];
}MOUNT;

