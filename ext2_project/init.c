#include <stdio.h>

#include <global.h>
#include <transfer.h>


MINODE *root;
PROC *running; // Points at the PROC structure of the current running process
// Every file operation is performed by the current running process

MINODE    MemoryInodeTable[NMINODES];
MOUNT     MountTable[NMOUNT];
PROC      ProcessTable[NPROC]; 
OPEN_FILE OpenFileTable[NOFT];

// ------------------------------------------------------------------------
// PROC *running           MINODE *root                          
//       |                          |              
//       |                          |               ||*********************
//       V                          |  MINODE       || 
//     PROC[0]                      V minode[100]   ||         Disk dev
//  =============  |-pointerToCWD-> ============    ||   ==================
//  |nextProcPtr|  |                |  INODE   |    ||   |     INODEs   
//  |pid = 1    |  |                | -------  |    ||   ================== 
//  |uid = 0    |  |                | (dev,2)  |    || 
//  |cwd --------->|                | refCount |    ||*********************
//  |           |                   | dirty    |
//  |fd[10]     |                   |          |         
//  | ------    |                   |          |
//  | - ALL 0 - |                   |==========|         
//  | ------    |                   |  INODE   |          
//  | ------    |                   | -------  |   
//  =============                   | (dev,ino)|   
//                                  |  refCount|  
//    PROC[1]                       |  dirty   |   
//     pid=2                        |          | 
//     uid=1                        |==========|  
//     cwd ----> root minode        
// ------------------------------------------------------------------------

// Initialize the data file system structures
void initialize_fs()
{
    int i = 0;

    PROC* pp       = NULL; 
    OPEN_FILE* ofp = NULL; 
    MINODE* mip    = NULL; 
    MOUNT *mp      = NULL; 

    // Initialize all processes
    for(i = 0; i < NPROC; i++)
    {
        int j = 0;

        pp = &ProcessTable[i];

        pp->pid    = i;
        pp->uid    = 0;
        pp->gid    = 0;
        pp->cwd    = NULL;
        pp->next   = NULL;
        pp->status = FREE;

        for(j = 0; j < NFD; j++)
            pp->fd[j] = NULL;
    }

    // Initialize all open files
    for(i = 0; i < NOFT; i++)
    {
        ofp = &OpenFileTable[i];

        ofp->mode       = 0;
        ofp->refCount   = 0;
        ofp->offset     = 0;
        ofp->minode_ptr = NULL;
    }

    // Initialize all memory-inodes
    for(i = 0; i < NMINODES; i++)
    {
        mip = &MemoryInodeTable[i];

        mip->device    = 0;
        mip->ino       = 0;
        mip->refCount  = 0;
        mip->dirty     = false;
        mip->mounted   = false;
        mip->mount_ptr = NULL;
    }

    // Initialize all mounts
    for(i = 0; i < NMOUNT; i++)
    {
        mp = &MountTable[i];

        mp->device      = 0;
        mp->nblocks     = 0;
        mp->ninodes     = 0;
        mp->bmap        = 0;
        mp->imap        = 0;
        mp->inode_block = 0;
        mp->minode_ptr  = NULL;
        strcpy(mp->name, "\0");
        strcpy(mp->mount_name, "\0");
    }

    root = NULL;
}

// Mount a device containing a file system as the root device.
// Mounting a root device amounts to loading the root INODE on that
// device into memory and establishing it as the root directory /
void mount_root(char* device_name)
{
    // When a file system starts, it must mount a device on the Root DIR /
    // That device is called the root device.
    // So the first thing a file system must do is to mount-root.
    // MountTable[0] represents the mounted root device.

    SUPER *sp = NULL;
    GD    *gp = NULL;
    MOUNT *mp = NULL;

    // Open device for read/write
    int dev = 0;
    if((dev = open(device_name, O_RDWR)) < 0)
    {
        perror("open device");
        exit(1);
    }

    // Verify device is an Ext2 Filesystem
    if(!isExt2(dev))
    {
        fprintf(stderr, "%s does not use the ext2 filesystem\n", device_name);
        exit(1);
    }
    printf("OK\n");

    //print_super(dev);
    //print_gd(dev);
    //print_inode(dev, ROOT_INODE);

    sp = get_super(dev);
    gp = get_gd(dev);
    mp = &MountTable[0];

    // Get root inode
    root = iget(dev, 2); root->refCount++;

    mp->device      = dev;
    mp->nblocks     = sp->s_blocks_count;
    mp->ninodes     = sp->s_inodes_count;
    mp->bmap        = gp->bg_block_bitmap;
    mp->imap        = gp->bg_inode_bitmap;
    mp->inode_block = gp->bg_inode_table;
    mp->minode_ptr  = root;          // Think about!
    strcpy(mp->name, "root");               //CHANGE
    strcpy(mp->mount_name, "sir mounty");   //CHANGE

    //printf("nblocks = %d\n", mp->nblocks);
    //printf("ninodes = %d\n", mp->ninodes);
    //printf("bmap    = %d\n", mp->bmap);
    //printf("imap    = %d\n", mp->imap);
    //printf("iblk    = %d\n", mp->iblk);

    free(sp);
    free(gp);
}

