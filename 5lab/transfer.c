// James Jessen
// 10918967   
// 
// CptS 360

#include "transfer.h"

static const int SUPER_BLOCK_OFFSET = 1024;
static const int SUPER_BLOCK_SIZE = 1024;


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

INODE *get_inode(int device, int inode_number)
{
    SUPER* sp = get_super(device);
    GD* gp = get_gd(device);

    int inodes_per_block = get_inodes_per_block(device);

    // Inode count starts from 1 not 0, so need -1
    int block = (inode_number - 1) / inodes_per_block + gp->bg_inode_table;
    int index = (inode_number - 1) % inodes_per_block;

    INODE* inode_table = (INODE*)get_block(device, block);
    INODE* inode = (INODE*)malloc(sizeof(INODE));
    *inode = inode_table[index];

    free(sp);
    free(gp);
    free(inode_table);
    return inode;
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

    SUPER* sp = (SUPER*)malloc(SUPER_BLOCK_SIZE);

    lseek(device, (long)SUPER_BLOCK_OFFSET, 0);
    bytes_read = read(device, sp, SUPER_BLOCK_SIZE);

    if(bytes_read != SUPER_BLOCK_SIZE)
        perror("get_super: read");

    return sp;
}

void put_super(int device, SUPER* buf)
{
    int bytes_written;

    lseek(device, (long)SUPER_BLOCK_OFFSET, 0);
    bytes_written = write(device, buf, SUPER_BLOCK_SIZE);

    if(bytes_written != SUPER_BLOCK_SIZE)
        perror("put_super: write");

    free(buf);
}

GD* get_gd(int device)
{
    // On the next block(s) following the superblock, 
    // is the Block Group Descriptor Table.

    if(get_block_size(device) > SUPER_BLOCK_OFFSET + SUPER_BLOCK_SIZE)
        return (GD*)get_block(device, 1);
    else
        return (GD*)get_block(device, 2);
}

void put_gd(int device, GD* buf)
{
    if(get_block_size(device) > SUPER_BLOCK_OFFSET + SUPER_BLOCK_SIZE)
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

    int block_size = 1024 << sp->s_log_block_size;
    int inode_size = sp->s_inode_size;
    
    int inodes_per_block = block_size / inode_size;

    free (sp);
    return inodes_per_block;
}

//----------------------------------- 

bool isExt2(int device)
{
    if (get_magic(device) == 0xEF53)
        return true;
    else
        return false;
}



