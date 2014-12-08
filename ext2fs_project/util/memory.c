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
#ifdef DEBUG
    printf("\nlogical_balloc\n");
#endif
    int next_block = get_num_blocks(block_size, ip);
    int indirection = 0;
    int bno = 0;

#ifdef DEBUG
    printf("allocating logical block %d\n", next_block);
#endif

    int* buf = (int*)ip->i_block;
    do 
    {
        int index = get_logic_path_index(block_size, &next_block, &indirection);

#ifdef DEBUG
        printf("index = %d\n", index);
#endif

        bno = buf[index];
        if(bno == 0)
        {
            bno = balloc(device);
            buf[index] = bno;

            ip->i_blocks += block_size / 512;
        }

        printf("bno = %d\n", bno);

        int* tmp = buf;
        buf = (int*)get_block(device, bno);

        if(tmp != (int*)ip->i_block)
            free(tmp);
    }
    while(indirection > 0);

    free(buf);
    return bno;
}

// Frees the last-most allocated block
void logical_bfree(int device, INODE* ip)
{
#ifdef DEBUG
    printf("\nlogical_bfree\n");
#endif

    const int block_size = get_block_size(device);

    int last_block = get_num_blocks(block_size, ip) - 1;
    int indirection = 0;

#ifdef DEBUG
    printf("freeing logical block %d\n", last_block);
#endif

    int* buf = (int*)ip->i_block;
    do 
    {
        int index = get_logic_path_index(block_size, &last_block, &indirection);
#ifdef DEBUG
        printf("index = %d\n", index);
#endif

        int bno = buf[index];

        int* prev = buf;
        buf = (int*)get_block(device, bno);

        prev[index] = 0;
        bfree(device, bno);
#ifdef DEBUG
        printf("freeing block %d\n", bno);
#endif
        ip->i_blocks -= block_size / 512;

        if(prev != (int*)ip->i_block)
            free(prev);
    }
    while(indirection > 0);
    free(buf);
}
