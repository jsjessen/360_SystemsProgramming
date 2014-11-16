// James Jessen
// 10918967   
// 
// CptS 360

// ----------------------------------------
// Global definitions, types, and variables 
// ----------------------------------------

#ifndef __GLOBAL_H_
#define __GLOBAL_H_


#include <stdio.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>

#define EMPTY 0
#define MAX_FILE_NAME_LENGTH 256
#define NUM_DIRECT_BLOCKS 12
#define NUM_DATA_BLOCKS   15

#define BLOCK_SIZE     1024
#define SUPER_SIZE     1024 
#define SUPER_OFFSET   1024  

// Block number of EXT2 FS on FD
#define SUPERBLOCK        1
#define GDBLOCK           2
#define ROOT_INODE        2

// Default dir and regular file modes
#define DIR_MODE    0040775 
#define FILE_MODE   0100664
#define SUPER_MAGIC  0xEF53
#define SUPER_USER        0

// Table sizes
#define NMINODES        100
#define NMOUNT           10
#define NPROC            10
#define NFD              10
#define NOFT            100


typedef enum { false, true } bool;

typedef enum { FAILURE, SUCCESS } result;

typedef enum { FREE, READY, RUNNING} process_status;

typedef unsigned char  u8;
typedef unsigned short u16; 
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR; 

// In-Memory Inode 
typedef struct minode
{
    INODE         inode;     // Inode structure on disk (data)
    int           device;    // The inode came from this device (for writing)
    int           ino;       // The inode came from inode # (for writing)
    int           refCount;  // # of processes using me
    int           dirty;     // Has my inode been modified? (eventually write to disk)
    bool          mounted;   // Have I been mounted? (I should be a DIR)
    struct mount *mount_ptr; // If mounted, points to the MountTable entry
}MINODE;

// Entry for MountTable
// A device that has been mounted (on a DIR)
typedef struct mount
{
    int     device;         // Which device am I?
    int     nblocks;        // # of blocks   ----------|
    int     ninodes;        // # of inodes             |   
    int     bmap;           // Block bitmap block #    |--> Convenient quick ref
    int     imap;           // Inode bitmap block #    |     
    int     inode_block;    // Inode block # ----------|
    MINODE *minode_ptr;     // Points to the DIR I'm mounted on
    char    name[64];       // What's the difference?
    char    mount_name[64]; // What's the difference?
}MOUNT;

// Entry for OpenFileTable
// An instance of an opened file
typedef struct open_file 
{
    int     mode;
    int     refCount;
    int     offset;
    MINODE *minode_ptr; // Whenever a file is ref, its inode must be brought into memory
}OPEN_FILE;

// A Process
typedef struct proc
{
    int          pid;      // Process ID
    int          uid;      // User ID (0 for superuser, non-zero for ordinary user)
    int          gid;      // Group ID
    int          status;   // Free, ready, or running
    MINODE      *cwd;      // -> CurrentWorkingDirectory in memory (initialized to root)
    OPEN_FILE   *fd[NFD];
    struct proc *next;     // Added so processes can form a circular linked list
}PROC;


// Global Variables
extern MINODE *root;
extern PROC *running; // Points at the PROC structure of the current running process
                      // Every file operation is performed by the current running process

extern MINODE    MemoryInodeTable[];
extern MOUNT     MountTable[];
extern PROC      ProcessTable[]; 
extern OPEN_FILE OpenFileTable[];


#endif
