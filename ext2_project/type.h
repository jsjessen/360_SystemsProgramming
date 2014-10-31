#include <stdio.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>

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

typedef unsigned char  u8;
typedef unsigned short u16; 
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR; 

typedef enum { false, true } bool;

// Open File Table
typedef struct oft
{
    int   mode;
    int   refCount;
    struct minode *inode_ptr;
    int   offset;
}OFT;

// Process structure
typedef struct proc
{
    int   pid;      // Process ID
    int   uid;      // User ID
    int   gid;      // Group ID
    int   status;
    OFT   *fd[NFD];
    struct minode *cwd; // Current working directory inode in memory
    struct proc *next; // processes form circular linked list
}PROC;

// In-memory Inodes structure
typedef struct minode
{
    INODE inode;    // Disk inode
    int   dev;      // Device
    int   ino;      // Inode #
    int   refCount; // # of references
    int   dirty;    // different than disk copy
    int   mounted;
    struct mount *mount_ptr;
}MINODE;

// Mount Table structure
typedef struct mount
{
    int    dev;     // Device
    int    nblocks; // # of blocks
    int    ninodes; // # of inodes
    int    bmap;    // Block bitmap
    int    imap;    // Inode bitmap
    int    iblk;
    MINODE *mounted_inode;
    char   name[64]; 
    char   mount_name[64];
}MOUNT;
