// James Jessen
// 10918967   
// 
// CptS 360

#include "print.h"


// *************** Title ***************  
void print_title(char* title, char symbol)
{
    int length = strlen(title) + 2; // space before/after title

    putchar('\n');
    for (int i = 0; i < TITLE_WIDTH - length; i++)
    {
        if (i != (TITLE_WIDTH - length) / 2)
            putchar(symbol);
        else
            printf(" %s ", title);
    }
    putchar('\n');
}

// -------------------------------------
void print_divider(char symbol)
{
    for (int i = 0; i < TITLE_WIDTH - 1; i++)
        putchar(symbol);

    putchar('\n');
}

// Print the relevant properties of the Super block in group 0
void print_super(int device)
{
    SUPER* sp = get_super(device); 

    print_title("Super Block", '=');
    printf("inodes_count            =                    %4u\n", sp->s_inodes_count);
    printf("blocks_count            =                    %4u\n", sp->s_blocks_count);
    printf("free_inodes_count       =                    %4u\n", sp->s_free_inodes_count);
    printf("free_blocks_count       =                    %4u\n", sp->s_free_blocks_count);
    printf("first_data_block        =                    %4u\n", sp->s_first_data_block);
    printf("log_block_size          =                    %4u\n", sp->s_log_block_size);
    printf("blocks_per_group        =                    %4u\n", sp->s_blocks_per_group);
    printf("inodes_per_group        =                    %4u\n", sp->s_inodes_per_group);
    printf("mnt_count               =                    %4u\n", sp->s_mnt_count);
    printf("max_mnt_count           =                    %4u\n", sp->s_max_mnt_count);
    printf("magic                   =                    %4x\n", sp->s_magic);
    printf("inode_size              =                    %4u\n", sp->s_inode_size);
    print_divider('-');

    free(sp);
}

// Print the relevant properties of the Group Descriptor block in group 0
void print_gd(int device)
{
    GD* gp = get_gd(device);

    print_title("Group Descriptor", '=');
    printf("block_bitmap            =                    %4u\n", gp->bg_block_bitmap);
    printf("inode_bitmap            =                    %4u\n", gp->bg_inode_bitmap);
    printf("inode_table             =                    %4u\n", gp->bg_inode_table);
    printf("free_blocks_count       =                    %4u\n", gp->bg_free_blocks_count);
    printf("free_inodes_count       =                    %4u\n", gp->bg_free_inodes_count);
    printf("used_dirs_count         =                    %4u\n", gp->bg_used_dirs_count);
    print_divider('-');

    free(gp);
}

// Print the relevant properties of a given inode 
void print_inode(int device, int inode_number)
{
    INODE *ip = get_inode(device, inode_number);

    if(inode_number == ROOT_INODE)
        print_title("Root Inode", '=');
    else
        print_title("Inode", '=');

    printf("inode                   =                    %4d\n", inode_number);
    printf("mode                    =                    %4x\n", ip->i_mode);
    printf("uid                     =                    %4u\n", ip->i_uid);
    printf("gid                     =                    %4u\n", ip->i_gid);
    printf("size                    =                    %4u\n", ip->i_size);
    printf("links_count             =                    %4u\n", ip->i_links_count);
    printf("i_block[0]              =                    %4u\n", ip->i_block[0]);
    print_divider('-');

    free(ip);
}

// Print the contents of a directory
void print_dir(int device, int inode_number)
{
    INODE *ip = get_inode(device, inode_number);
    int block_size = get_block_size(device);

    printf("\n********** DIR OF INODE: %d **********\n", inode_number);
    for(int i = 0; i < (ip->i_size / block_size); i++)
    {
        if (ip->i_block[i] == EMPTY || i >= NUM_DIRECT_BLOCKS)
            break;

        u8* block = get_block(device, ip->i_block[i]);
        u8* cp = block; 
        DIR* dp = (DIR*)block;

        printf("i_block[%d]\n", i);
        print_divider('-');
        printf(" inode        rec_len      name_len     name\n");
        print_divider('-');

        while (cp < block + block_size)
        {
            char name[256];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = 0;
            printf(" %4d          %4d          %4d        %s\n",
                    dp->inode, dp->rec_len, dp->name_len, name);

            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 
        putchar('\n');
        free(block);
    }
}

// Print the Block Bitmap of group 0
void print_bmap(int device)
{
    int i;
    u8* bmap = get_bmap(device);
    int blocks_count = get_blocks_count(device);

    print_title("Block Bitmap", '=');
    for(i = 0; i < blocks_count; i++)
    {
        if(i && (i % (GROUPS_PER_LINE * GROUP_SIZE)) == 0)
            putchar('\n');
        else if(i && (i % GROUP_SIZE) == 0)
            putchar(' ');

        (test_bit(bmap, i)) ? putchar('1') : putchar('0');
    }
    print_divider('-');

    free(bmap);
}

// Print the Inode Bitmap of group 0
void print_imap(int device)
{
    int i;
    u8* imap = get_imap(device);
    int inode_count = get_inodes_count(device);

    print_title("Inode Bitmap", '=');
    for(i = 0; i < inode_count; i++)
    {
        if(i && (i % (GROUPS_PER_LINE * GROUP_SIZE)) == 0)
            putchar('\n');
        else if(i && (i % GROUP_SIZE) == 0)
            putchar(' ');

        (test_bit(imap, i)) ? putchar('1') : putchar('0');
    }
    print_divider('-');

    free(imap);
}

// Print the data blocks of a file/inode
void print_file_blocks(int device, int inode_number)
{
    int i;
    int block_size = get_block_size(device);
    INODE *ip = get_inode(device, inode_number);

    // Disk Blocks - list i_block[] non-zeroes
    print_title("Inode Blocks", '=');
    for (i = 0; i < 15 && ip->i_block[i] > 0; i++)
        printf("block[%2d] = %d\n", i, ip->i_block[i]);

    getchar();
    print_title("Direct Blocks", '=');
    for (i = 0; i < 12; i++)
    {
        if (i && i % GROUPS_PER_LINE == 0)
            putchar('\n');
        else if(i)   
            putchar(' ');

        if (ip->i_block[i] == 0)
        {
            printf("\n\n");
            return;
        }
        else
            printf("%4d", ip->i_block[i]);
    }
    putchar('\n');

    getchar();
    print_title("Indirect Blocks", '=');
    printf(" %d", ip->i_block[12]);
    if(!print_indirect_block(device, block_size, 
                (int*)get_block(device, ip->i_block[12]), 1))
    {
        free(ip);
        return;
    }
    putchar('\n');

    getchar();
    print_title("Double Indirect Blocks", '=');
    printf(" %d", ip->i_block[13]);
    if(!print_indirect_block(device, block_size, 
                (int*)get_block(device, ip->i_block[13]), 2))
    {
        free(ip);
        return;
    }
    putchar('\n');

    getchar();
    print_title("Triple Indirect Blocks", '=');
    printf(" %d", ip->i_block[14]);
    if(!print_indirect_block(device, block_size, 
                (int*)get_block(device, ip->i_block[14]), 3))
    {
        free(ip);
        return;
    }
    putchar('\n');
}

// Recursive function for printing indirect data blocks
int print_indirect_block(int device, int block_size, int* buf, int level)
{
    if(level - 1)
    {
        for(int i = 0; i < block_size / sizeof(int); i++)
        {
            printf(" -> %d", buf[i]);
            if(!print_indirect_block(device, block_size, 
                        (int*)get_block(device, buf[i]), level - 1))
            {
                free(buf);
                return 0;
            }
        }
    }
    else
    {
        printf(":\n");
        print_divider('-');

        for(int i = 0; i < block_size / sizeof(int); i++)
        {
            if (i && i % GROUPS_PER_LINE == 0)
                putchar('\n');
            else if(i)   
                putchar(' ');

            if (buf[i] == 0)
            {
                printf("\n\n");
                free(buf);
                return 0;
            }
            else
                printf("%4d", buf[i]);
        }
    }

    free(buf);
    return 1;
}
