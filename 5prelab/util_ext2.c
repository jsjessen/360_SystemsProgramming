/* James Jessen             
 * 10918967                
 *
 * CptS 360
 */

#include "util_ext2.h"

static const int SUPER_BLOCK_OFFSET = 1024;
static const int SUPER_BLOCK_SIZE = 1024;

u8* get_block(int fd, int block)
{
    int block_size = get_block_size(fd); 
    u8* buf = (u8*)malloc(block_size);

    lseek(fd, (long)(block * block_size), 0);
    read(fd, buf, block_size);

    return buf;
}

INODE* get_inode(int fd, int inode)
{
    GD* gp = get_gd(fd);
    INODE* ip = (INODE*)get_block(fd, gp->bg_inode_table);

    free(gp);
    return ip + (inode - 1);
}

SUPER* get_super(int fd) 
{
    // An Ext2 file systems starts with a superblock 
    // located at byte offset 1024 from the start of the volume. 

    // This is block 1 for a 1KiB block formatted volume or 
    // within block 0 for larger block sizes. 

    // Note that the size of the superblock is constant 
    // regardless of the block size.

    SUPER* sp = (SUPER*)malloc(SUPER_BLOCK_SIZE);

    lseek(fd, (long)SUPER_BLOCK_OFFSET, 0);
    read(fd, sp, SUPER_BLOCK_SIZE);

    return sp;
}

GD* get_gd(int fd)
{
    // On the next block(s) following the superblock, 
    // is the Block Group Descriptor Table.

    if(get_block_size(fd) > SUPER_BLOCK_OFFSET + SUPER_BLOCK_SIZE)
        return (GD*)get_block(fd, 1);
    else
        return (GD*)get_block(fd, 2);
}

u8* get_block_bitmap(int fd)
{
    GD* gp = get_gd(fd);
    u8* bmap = get_block(fd, gp->bg_block_bitmap);

    free(gp);
    return bmap;
}

u8* get_inode_bitmap(int fd)
{
    GD* gp = get_gd(fd);
    u8* imap = get_block(fd, gp->bg_inode_bitmap);

    free(gp);
    return imap;
}

INODE** get_inode_table(int fd)
{
    GD* gp = get_gd(fd);
    INODE** inode_table = (INODE**)get_block(fd, gp->bg_inode_table);

    free(gp);
    return inode_table;
}

//-------------------------------------------------- 

int get_magic(int fd)
{
    SUPER* sp = get_super(fd);
    int magic = sp->s_magic;

    free(sp);
    return magic;
}

int get_block_size(int fd)
{
    SUPER* sp = get_super(fd);
    int block_size = 1024 << sp->s_log_block_size;

    free (sp);
    return block_size;
}
int get_blocks_count(int fd)
{
    SUPER* sp = get_super(fd);
    int blocks_count = sp->s_blocks_count;

    free(sp);
    return blocks_count;
}

int get_inodes_count(int fd)
{
    SUPER* sp = get_super(fd);
    int inodes_count = sp->s_inodes_count;

    free(sp);
    return inodes_count;
}

//-------------------------------------------------- 

int test_bit(u8* buf, int bit)
{
    int byte = bit / 8; 
    bit %= 8;

    if (buf[byte] & (1 << bit))
        return 1;
    else
        return 0;
}

void set_bit(u8* buf, int bit)
{
    int byte = bit / 8; 
    bit %= 8;

    buf[byte] |= (1 << bit);
}

void clear_bit(u8* buf, int bit)
{
    int byte = bit / 8; 
    bit %= 8;

    buf[byte] &= ~(1 << bit);
}
