// James Jessen
// 10918967   
// 
// CptS 360

#include "transfer.h"

static const int SUPER_BLOCK_OFFSET = 1024;
static const int SUPER_BLOCK_SIZE = 1024;


void put_block(int dev, int block, u8* buf)
{
    int bytes_written;
    int block_size = get_block_size(dev); 

    lseek(dev, (long)(block * block_size), 0);
    bytes_written = write(dev, buf, block_size);

    if(bytes_written != block_size)
        perror("put_block: write");

    free(buf);
}

u8* get_block(int dev, int block)
{
    int bytes_read;
    int block_size = get_block_size(dev); 
    u8* buf = (u8*)malloc(block_size);

    lseek(dev, (long)(block * block_size), 0);
    bytes_read = read(dev, buf, block_size);

    if(bytes_read != block_size)
        perror("get_block: read");

    return buf;
}

// VERIFY
INODE *get_inode(int dev, int inode_number)
{
    SUPER* sp = get_super(dev);
    GD* gp = get_gd(dev);

    int block_group = (inode_number - 1) / sp->s_inodes_per_group;
    int local_index = (inode_number - 1) % sp->s_inodes_per_group;

    int block = (block_group * sp->s_blocks_per_group) + gp->bg_inode_table;

    INODE* inode_table = (INODE*)get_block(dev, block);
    INODE* inode = (INODE*)malloc(sizeof(INODE));
    *inode = inode_table[local_index];

    free(sp);
    free(gp);
    free(inode_table);
    return inode;
}

SUPER* get_super(int dev) 
{
    // An Ext2 file systems starts with a superblock 
    // located at byte offset 1024 from the start of the volume. 

    // This is block 1 for a 1KiB block formatted volume or 
    // within block 0 for larger block sizes. 

    // Note that the size of the superblock is constant 
    // regardless of the block size.
    int bytes_read;

    SUPER* sp = (SUPER*)malloc(SUPER_BLOCK_SIZE);

    lseek(dev, (long)SUPER_BLOCK_OFFSET, 0);
    bytes_read = read(dev, sp, SUPER_BLOCK_SIZE);

    if(bytes_read != SUPER_BLOCK_SIZE)
        perror("get_super: read");

    return sp;
}

void put_super(int dev, SUPER* buf)
{
    int bytes_written;

    lseek(dev, (long)SUPER_BLOCK_OFFSET, 0);
    bytes_written = write(dev, buf, SUPER_BLOCK_SIZE);

    if(bytes_written != SUPER_BLOCK_SIZE)
        perror("put_super: write");

    free(buf);
}

GD* get_gd(int dev)
{
    // On the next block(s) following the superblock, 
    // is the Block Group Descriptor Table.

    if(get_block_size(dev) > SUPER_BLOCK_OFFSET + SUPER_BLOCK_SIZE)
        return (GD*)get_block(dev, 1);
    else
        return (GD*)get_block(dev, 2);
}

void put_gd(int dev, GD* buf)
{
    if(get_block_size(dev) > SUPER_BLOCK_OFFSET + SUPER_BLOCK_SIZE)
        put_block(dev, 1, (u8*)buf);
    else
        put_block(dev, 2, (u8*)buf);
}

u8* get_bmap(int dev)
{
    GD* gp = get_gd(dev);
    u8* bmap = get_block(dev, gp->bg_block_bitmap);

    free(gp);
    return bmap;
}

void put_bmap(int dev, u8* buf)
{
    GD* gp = get_gd(dev);
    put_block(dev, gp->bg_block_bitmap, buf);

    free(gp);
}

u8* get_imap(int dev)
{
    GD* gp = get_gd(dev);
    u8* imap = get_block(dev, gp->bg_inode_bitmap);

    free(gp);
    return imap;
}

void put_imap(int dev, u8* buf)
{
    GD* gp = get_gd(dev);
    put_block(dev, gp->bg_inode_bitmap, buf);

    free(gp);
}

//-------------------------------------------------- 

int get_magic(int dev)
{
    SUPER* sp = get_super(dev);
    int magic = sp->s_magic;

    free(sp);
    return magic;
}

int get_block_size(int dev)
{
    SUPER* sp = get_super(dev);
    int block_size = 1024 << sp->s_log_block_size;

    free (sp);
    return block_size;
}
int get_blocks_count(int dev)
{
    SUPER* sp = get_super(dev);
    int blocks_count = sp->s_blocks_count;

    free(sp);
    return blocks_count;
}

int get_inodes_count(int dev)
{
    SUPER* sp = get_super(dev);
    int inodes_count = sp->s_inodes_count;

    free(sp);
    return inodes_count;
}

//----------------------------------- 

bool isExt2(int device)
{
    if (get_magic(device) == 0xEF53)
        return true;
    else
        return false;
}



