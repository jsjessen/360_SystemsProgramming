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
    if(inode <= 0)
        return;

    u8* imap = get_imap(dev);

    clear_bit(&imap, inode - 1);
    set_free_inodes(dev, +1);

    put_imap(dev, imap);
}

void bfree(int dev, int block)
{
    if(block <= 0)
        return;

    u8* bmap = get_bmap(dev);

    clear_bit(&bmap, block);
    set_free_blocks(dev, +1);

    put_bmap(dev, bmap);
}

// Allocates the next available block
int logical_balloc(int device, INODE* ip)
{
    const int block_size = get_block_size(device);

    int next_block = get_num_blocks(block_size, ip);
    int indirection = 0;
    int bno = 0;

    int* buf = (int*)ip->i_block;
    do 
    {
        int index = get_logic_path_index(block_size, &next_block, &indirection);

        bno = buf[index];

        if(bno == 0)
            bno = balloc(device);

        int* tmp = buf;
        buf = (int*)get_block(device, bno);

        if(tmp != (int*)ip->i_block)
            free(tmp);
    }
    while(indirection > 0);

    return bno;
}

// Frees the last-most allocated block
void logical_bfree(int device, INODE* ip)
{
    // parallel
    int free_index[4];
    int free_bno[4];
    int num_index = 0;

    const int block_size = get_block_size(device);

    int last_block = get_num_blocks(block_size, ip) - 1;
    int indirection = 0;

    int* buf = (int*)ip->i_block;
    do 
    {
        int index = get_logic_path_index(block_size, &last_block, &indirection);

        if(index == 0)
        {
            free_index[num_index] = index;
            free_bno[num_index] = buf[index];
            num_index++;
        }

        int* tmp = buf;
        buf = (int*)get_block(device, buf[index]);

        if(tmp != (int*)ip->i_block)
            free(tmp);
    }
    while(indirection > 0);

    while(num_index >= 0)
    {
        if(free_index[num_index] == 0)
            bfree(device, free_bno[num_index]);
        else
            break;

        num_index--;
    }
}
