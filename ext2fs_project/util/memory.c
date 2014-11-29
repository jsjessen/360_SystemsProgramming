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



int simple_pow(int base, int power)
{
    int result = 1; 

    while(power-- > 0)
        result *= base; 

    return result;
}

// Returns the index for the current block, on the path to the logical block
// Use: Initially call this function with indirection less than or equal to zero
//      and it will return the index for i_block[?] and update the parameter values
//      Continue calling this function with the same variables until indirection equals zero

// Use: Call this function in a loop until indirection is zero
// Returns pointer to block number on the path to the block 
int rlogic(int block_size, int* logical_block, int* indirection)
{
    int int_per_block = block_size / sizeof(int);

    // Initial case: determine i_block[?]
    if(*indirection <= 0)
    {
        // Direct blocks
        if(*logical_block < NUM_DIRECT_BLOCKS)
        {
            *indirection = 0;
            *logical_block = 0;

            return *logical_block; 
        }

        for(*indirection = 1; *indirection <= 3; *indirection++)
        {
            // Indirect blocks 
            if(*logical_block < NUM_DIRECT_BLOCKS + simple_pow(int_per_block, *indirection))
            {
                *logical_block -= NUM_DIRECT_BLOCKS;

                for(int i = *indirection; i > 0; --i)
                    *logical_block -= simple_pow(int_per_block, i);

                return (NUM_DIRECT_BLOCKS + *indirection) - 1;
            }
        }
    }

    *indirection--;

    int index = *logical_block / simple_pow(int_per_block, *indirection);

    *logical_block -= index * simple_pow(int_per_block, *indirection);
    return index;
}

    int get_num_blocks(int block_size, INODE* ip)
{
    return ip->i_blocks / (block_size / 512);
}

// Allocates the next available block
int alloc_logical_block(int device, INODE* ip)
{
    const int block_size = get_block_size(device);

    int num = get_num_blocks(block_size, ip);
    int* next_block = &num; 
    int* indirection = 0;

    int* cur_buf = ip->i_block;
    do 
    {
        int index = rlogic(block_size, next_block, indirection);

        if(cur_buf[index] == 0)
            cur_buf[index] = balloc(device);

        int* tmp = cur_buf;
        cur_buf = (int*)get_block(device, cur_buf[index]);

        if(tmp != (int*)ip->i_block)
            free(cur_buf);
    }
    while(indirection > 0);
}

// Frees the last-most allocated block
void free_logical_block(int device, INODE* ip)
{
    // parallel
    int free_index[4];
    int free_bno[4];
    int num_index = 0;

    const int block_size = get_block_size(device);

    int num = get_num_blocks(block_size, ip) - 1;
    int* last_block = &num; 
    int* indirection = 0;

    int* cur_buf = ip->i_block;
    do 
    {
        int index = rlogic(block_size, last_block, indirection);

        if(index == 0)
        {
            free_index[num_index] = index;
            free_bno[num_index] = cur_buf[index];
            num_index++;
        }

        int* tmp = cur_buf;
        cur_buf = (int*)get_block(device, cur_buf[index]);

        if(tmp != (int*)ip->i_block)
            free(cur_buf);
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


