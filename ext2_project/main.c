/* James Jessen
 * 10918967   
 *
 * CptS 360
 */

#include <stdio.h>

#include "type.h"
#include "input.h"
#include "print.h"
#include "cmd.h"

MINODE minode[NMINODES];
MINODE *root;

int fd;
char buf[1024];
int dev;
char *rootdev;
char *pathname;
char *parameter;

MINODE *root;
PROC *running; // Points at the PROC structure of the current running process
               // Every file operation is performed by the current running process

MINODE    MemoryInodeTable[NMINODES];
MOUNT     MountTable[NMOUNT];
PROC      ProcessTable[NPROC]; //PROC P[NPROC];
OPEN_FILE OpenFileTable[NOFT];

// Initialize the data file system structures
void fs_initialize(void)
{
    int i = 0;

    running = &ProcessTable[0];

    // Initialize all processes
    for(i = 0; i < NPROC; i++)
    {
        int j = 0;

        PROC* pp = &ProcessTable[i];

        pp->pid    = i;
        pp->uid    = 0;
        pp->gid    = 0;
        pp->cwd    = NULL;
        pp->next   = NULL;
        pp->status = FREE;

        for(j = 0; j < NFD; j++)
            pp->fd[j] = NULL;
    }

    ProcessTable[0].uid = 0; // Super-user
    ProcessTable[1].uid = 1; // Ordinary-user

    // Processes form a circularly linked list
    ProcessTable[0].next = &ProcessTable[1];
    ProcessTable[1].next = &ProcessTable[0];

    // Initialize all open files
    for(i = 0; i < NOFT; i++)
    {
        OPEN_FILE* ofp = &OpenFileTable[i];

        ofp->mode       = 0;
        ofp->refCount   = 0;
        ofp->offset     = 0;
        ofp->minode_ptr = NULL

    }

    // Initialize all memory-inodes
    for(i = 0; i < NMINODES; i++)
    {
        MINODE* mip = &MemoryInodeTable[i];

        mip->dev      = 0;
        mip->ino      = 0;
        mip->refCount = 0;
        mip->dirty    = false;
        mip->mounted  = false;
        mip->mptr     = NULL;
    }

    // Initialize all mounts
    for(i = 0; i < NMOUNT; i++)
    {
        MOUNT *mp = &MountTable[i];

        mp->dev        = 0;
        mp->nblocks    = 0;
        mp->ninodes    = 0;
        mp->bmap       = 0;
        mp->imap       = 0;
        mp->iblk       = 0;
        mp->minode_ptr = NULL;
        mp->name       = "\0";
        mp->mount_name = "\0";
    }

    root = NULL;
}

// Mount a device containing a file system as the root device.
// Mounting a root device amounts to loading the root INODE on that
// device into memory and establishing it as the root directory /
int mount_root(char* device_name)
{
    // When a file system starts, it must mount a device on the Root DIR /
    // That device is called the root device.
    // So the first thing a file system must do is to mount-root.
    // MountTable[0] represents the mounted root device.

    int dev = 0;

    SUPER *sp = NULL
    GD    *gp = NULL;
    MOUNT *mp = NULL;

    // Open device for read/write
    if((dev = open(device_name, O_RDWR)) < 0)
    {
        perror("open device");
        exit(1);
    }

    // Verify device is an Ext2 Filesystem
    if(!isExt2(dev))
    {
        fprintf(stderr, "%s does not use the ext2 filesystem\n", device);
        exit(1);
    }

    sp = get_super(dev);
    gp = get_gd(dev);
    mp = &MountTable[0];

    // Get root inode
    root = iget(dev, 2); root->refCount++;
    
    ProcessTable[0].cwd = root; root->refCount++;
    ProcessTable[1].cwd = root; root->refCount++;

    mp->dev = dev;
    mp->nblocks = sp->s_blocks_count;
    mp->ninodes = sp->s_inodes_count;
    mp->bmap = gp->bg_block_bitmap;
    mp->imap = gp->bg_inode_bitmap;
    mp->iblk = gp->bg_inode_table;
    mp->minode_ptr = root;          // Think about!
    mp->name = "root";              // CHANGE
    mp->mount_name = "sir mounty";  // CHANGE
}

int main(int argc, char* argv[])
{
    int   fd = -1;
    char* device = NULL;

    if (argc != 2)
    {
        printf("Usage:\n");
        printf("         myext2   DEVICE\n");
        printf("  e.g.   myext2   mydisk\n\n");
        exit(1);
    }

    device = argv[1];

    fs_initialize(); 
    mount_root(device);

    while(true)
    {
        char*  input    = NULL;
        char** cmd_argv = NULL;
        int    cmd_argc = 0;
        int  (*cmd_fptr = NULL)(int, char**); 

        do { printf("command : "); }     // Prompt user for input
        while(!(input = get_input()));   // Get user input

        cmd_argv = parse(input, " ");    // Parse input into cmd argv[]

        while(cmd_argv[cmd_argc + 1])    // Determine cmd argc
        { cmd_argc++; } 

        cmd_fptr = get_cmd(cmd_argv[0]); // Get the command's function pointer
        cmd_fptr(cmd_argc, cmd_argv);    // Execute the command with parameters

        free(input);
        free_array(cmd_argv);
    }

    return 0;
}
