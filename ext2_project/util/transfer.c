// James Jessen
// 10918967   
// 
// CptS 360

#include "transfer.h"


u8* get_block(int device, int block)
{
    int bytes_read;
    int block_size = get_block_size(device); 
    u8* buf = (u8*)malloc(block_size);

    lseek(device, (long)(block * block_size), 0);
    bytes_read = read(device, buf, block_size);

    if(bytes_read != block_size)
        perror("get_block: read");

    return buf;
}

void put_block(int device, int block, u8* buf)
{
    int bytes_written;
    int block_size = get_block_size(device); 

    lseek(device, (long)(block * block_size), 0);
    bytes_written = write(device, buf, block_size);

    if(bytes_written != block_size)
        perror("put_block: write");

    free(buf);
}


INODE *get_inode(int device, int inode_number)
{
    int inodes_per_block = get_inodes_per_block(device);
    int inode_table_block = get_inode_table_block(device);

    // Inode count starts from 1 not 0, so need -1
    int block = (inode_number - 1) / inodes_per_block + inode_table_block;
    int index = (inode_number - 1) % inodes_per_block;

    INODE* inode_table = (INODE*)get_block(device, block);
    INODE* inode = (INODE*)malloc(sizeof(INODE));
    *inode = inode_table[index];

    free(inode_table);
    return inode;
}

void put_inode(int device, int inode_number, INODE inode)
{
    int inode_table_block = get_inode_table_block(device);
    int inodes_per_block = get_inodes_per_block(device);

    // Inode count starts from 1 not 0, so need -1
    int block = (inode_number - 1) / inodes_per_block + inode_table_block;
    int index = (inode_number - 1) % inodes_per_block;

    INODE* inode_table = (INODE*)get_block(device, block);
    inode_table[index] = inode;
    put_block(device, block, (u8*)inode_table);
}


MINODE *iget(int device, int inode_number)
{
    // Once you have the ino of an inode, you may load the inode into a slot
    // in the Minode[] array. 

    if(inode_number < ROOT_INODE)
        return NULL;

    // To ensure uniqueness, you must search the Minode[] 
    // array to see whether the needed INODE already exists
    for(int i = 0; i < NMINODES; i++)
    {
        MINODE* mip = &MemoryInodeTable[i];

        if(mip->device == device && mip->ino == inode_number)
        {
            // If you find the needed INODE already in a Minode[] slot, just inc its 
            // refCount by 1 and return the Minode[] pointer.
            mip->refCount++;
            return mip;
        }
    }

    // If you do not find it in memory
    for(int i = 0; i < NMINODES; i++)
    {
        MINODE* mip = &MemoryInodeTable[i];

        if(mip->refCount == 0)
        {
            INODE* ip = NULL;

            // you must allocate a FREE Minode[i], 
            // load the INODE from disk into that Minode[i].INODE, 
            // initialize the Minode[]'s other fields 

            mip->device    = device;
            mip->ino       = inode_number;
            mip->refCount  = 1;
            mip->dirty     = false;
            mip->mounted   = false;
            mip->mount_ptr = NULL;

            ip = get_inode(device, inode_number);

            // copy INODE to mp->INODE
            mip->inode = *ip;
            free(ip);

            // return its address as a MINODE pointer,
            return mip;
        }
    }

    printf("No available inodes in memory!\n");
    return NULL;
}

void iput(MINODE *mip)
{
    if(!mip) return;

    // Decrement refCount
    // Only write to MemoryInode to disk if
    // No processes are using it and it has been modified
    if(--(mip->refCount) <= 0 && mip->dirty == true)
    {
        put_inode(mip->device, mip->ino, mip->inode);
        mip->dirty = false;
    }
}


SUPER* get_super(int device) 
{
    // An Ext2 file systems starts with a superblock 
    // located at byte offset 1024 from the start of the volume. 

    // This is block 1 for a 1KiB block formatted volume or 
    // within block 0 for larger block sizes. 

    // Note that the size of the superblock is constant 
    // regardless of the block size.
    int bytes_read;

    SUPER* sp = (SUPER*)malloc(SUPER_SIZE);

    lseek(device, (long)SUPER_OFFSET, 0);
    bytes_read = read(device, sp, SUPER_SIZE);

    if(bytes_read != SUPER_SIZE)
        perror("get_super: read");

    return sp;
}

void put_super(int device, SUPER* buf)
{
    int bytes_written;

    lseek(device, (long)SUPER_OFFSET, 0);
    bytes_written = write(device, buf, SUPER_SIZE);

    if(bytes_written != SUPER_SIZE)
        perror("put_super: write");

    free(buf);
}


GD* get_gd(int device)
{
    // On the next block(s) following the superblock, 
    // is the Block Group Descriptor Table.

    if(get_block_size(device) > SUPER_OFFSET + SUPER_SIZE)
        return (GD*)get_block(device, 1);
    else
        return (GD*)get_block(device, 2);
}

void put_gd(int device, GD* buf)
{
    if(get_block_size(device) > SUPER_OFFSET + SUPER_SIZE)
        put_block(device, 1, (u8*)buf);
    else
        put_block(device, 2, (u8*)buf);
}


u8* get_bmap(int device)
{
    GD* gp = get_gd(device);
    u8* bmap = get_block(device, gp->bg_block_bitmap);

    free(gp);
    return bmap;
}

void put_bmap(int device, u8* buf)
{
    GD* gp = get_gd(device);
    put_block(device, gp->bg_block_bitmap, buf);

    free(gp);
}


u8* get_imap(int device)
{
    GD* gp = get_gd(device);
    u8* imap = get_block(device, gp->bg_inode_bitmap);

    free(gp);
    return imap;
}

void put_imap(int device, u8* buf)
{
    GD* gp = get_gd(device);
    put_block(device, gp->bg_inode_bitmap, buf);

    free(gp);
}

//-------------------------------------------------- 

int get_magic(int device)
{
    SUPER* sp = get_super(device);
    int magic = sp->s_magic;

    free(sp);
    return magic;
}

int get_block_size(int device)
{
    SUPER* sp = get_super(device);
    int block_size = 1024 << sp->s_log_block_size;

    free (sp);
    return block_size;
}

int get_inode_size(int device)
{
    SUPER* sp = get_super(device);
    int inode_size = sp->s_inode_size;

    free (sp);
    return inode_size;
}

int get_blocks_count(int device)
{
    SUPER* sp = get_super(device);
    int blocks_count = sp->s_blocks_count;

    free(sp);
    return blocks_count;
}

int get_inodes_count(int device)
{
    SUPER* sp = get_super(device);
    int inodes_count = sp->s_inodes_count;

    free(sp);
    return inodes_count;
}

int get_inodes_per_block(int device)
{
    SUPER* sp = get_super(device);

    int inodes_per_block = 0;

    int block_size = 1024 << sp->s_log_block_size;
    int inode_size = sp->s_inode_size;

    if(inode_size <= 0)
    {
        fprintf(stderr, "transfer.c: get_inodes_per_block(): inode_size = %d\n", inode_size);
        return FAILURE;
    }
    inodes_per_block = block_size / inode_size;

    free(sp);
    return inodes_per_block;
}

int get_inode_table_block(int device)
{
    GD* gp = get_gd(device);
    int inode_table_block = gp->bg_inode_table;

    free(gp);
    return inode_table_block;
}

//-------------------------------------------------- 

bool isExt2(int device)
{
    if (get_magic(device) == SUPER_MAGIC)
        return true;
    else
        return false;
}
