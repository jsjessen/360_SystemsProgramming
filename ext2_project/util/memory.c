// James Jessen
// 10918967   
// 
// CptS 360

#include "memory.h"

void free_array(char** array)
{
    int i = 0;

    if(!array)
        return;

    while(array[i])
        free(array[i++]);

    free(array);
}

int test_bit(u8* buf, int bit)
{
    int byte = bit / 8; 
    bit %= 8;

    if (buf[byte] & (1 << bit))
        return 1;
    else
        return 0;
}


void set_bit(u8** buf, int bit)
{
    int byte = bit / 8; 
    bit %= 8;

    (*buf)[byte] |= (1 << bit);
}

void clear_bit(u8** buf, int bit)
{
    int byte = bit / 8; 
    bit %= 8;

    (*buf)[byte] &= ~(1 << bit);
}


void set_free_inodes(int dev, int change)
{
    SUPER* sp = get_super(dev);
    sp->s_free_inodes_count += change;
    put_super(dev, sp);

    GD* gp = get_gd(dev);
    gp->bg_free_inodes_count += change;
    put_gd(dev, gp);
}

void set_free_blocks(int dev, int change)
{
    SUPER* sp = get_super(dev);
    sp->s_free_blocks_count += change;
    put_super(dev, sp);

    GD* gp = get_gd(dev);
    gp->bg_free_blocks_count += change;
    put_gd(dev, gp);
}

int ialloc(int dev)
{
    int inode_count = get_inodes_count(dev);
    u8* imap = get_imap(dev);

    int  i;
    for (i = 0; i < inode_count; i++)
    {
        if (test_bit(imap, i) == 0)
        {
            set_bit(&imap, i);
            set_free_inodes(dev, -1);
            put_imap(dev, imap);

            return i + 1;
        }
    }
    printf("ialloc(): no more free inodes\n");
    return -1;
}

int balloc(int dev)
{
    int block_count = get_blocks_count(dev);
    u8* bmap = get_bmap(dev);

    int  i;
    for (i = 0; i < block_count; i++)
    {
        if (test_bit(bmap, i) == 0)
        {
            set_bit(&bmap, i);
            set_free_blocks(dev, -1);
            put_bmap(dev, bmap);

            return i;
        }
    }
    printf("balloc(): no more free blocks\n");
    return -1;
}

void ifree(int dev, int inode)
{
    u8* imap = get_imap(dev);

    clear_bit(&imap, inode - 1);
    set_free_inodes(dev, +1);

    put_imap(dev, imap);
}

void bfree(int dev, int block)
{
    u8* bmap = get_bmap(dev);

    clear_bit(&bmap, block);
    set_free_blocks(dev, +1);

    put_bmap(dev, bmap);
}


