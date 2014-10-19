/* James Jessen             
 * 10918967                  
 *
 * CptS 360
 */

#include "print_ext2.h"

static const int ROOT_INODE_NUMBER = 2;

void print_super(int fd)
{
    SUPER* sp = get_super(fd); 

    printf("\n********** SUPER **********\n");
    printf("inodes_count       =  %u\n", sp->s_inodes_count);
    printf("blocks_count       =  %u\n", sp->s_blocks_count);
    printf("free_inodes_count  =  %u\n", sp->s_free_inodes_count);
    printf("free_blocks_count  =  %u\n", sp->s_free_blocks_count);
    printf("log_block_size     =  %u\n", sp->s_log_block_size);
    printf("blocks_per_group   =  %u\n", sp->s_blocks_per_group);
    printf("inodes_per_group   =  %u\n", sp->s_inodes_per_group);
    printf("mnt_count          =  %u\n", sp->s_mnt_count);
    printf("max_mnt_count      =  %u\n", sp->s_max_mnt_count);
    printf("magic              =  %x\n", sp->s_magic);
    printf("mtime              =  %s"  , ctime((time_t*)&sp->s_mtime));
    printf("inode_size         =  %u\n", sp->s_inode_size);

    free(sp);
}

void print_gd(int fd)
{
    GD* gp = get_gd(fd);

    printf("\n********** GROUP DESCRIPTOR **********\n");
    printf("block_bitmap       =  %u\n", gp->bg_block_bitmap);
    printf("inode_bitmap       =  %u\n", gp->bg_inode_bitmap);
    printf("inode_table        =  %u\n", gp->bg_inode_table);
    printf("free_blocks_count  =  %u\n", gp->bg_free_blocks_count);
    printf("free_inodes_count  =  %u\n", gp->bg_free_inodes_count);
    printf("used_dirs_count    =  %u\n", gp->bg_used_dirs_count);

    free(gp);
}

void print_bmap(int fd)
{
    u8* bmap = get_block_bitmap(fd);
    int blocks_count = get_blocks_count(fd);

    int i;
    printf("\n********** BLOCK BITMAP **********\n");
    for(i = 0; i < blocks_count; i++)
    {
        if(i && (i % (GROUPS_PER_LINE * GROUP_SIZE)) == 0)
            putchar('\n');
        else if(i && (i % GROUP_SIZE) == 0)
            putchar(' ');

        (test_bit(bmap, i)) ? putchar('1') : putchar('0');
    }
    putchar('\n');

    free(bmap);
}

void print_imap(int fd)
{
    u8* imap = get_inode_bitmap(fd);
    int inode_count = get_inodes_count(fd);

    int i;
    printf("\n********** INODE BITMAP **********\n");
    for(i = 0; i < inode_count; i++)
    {
        if(i && (i % (GROUPS_PER_LINE * GROUP_SIZE)) == 0)
            putchar('\n');
        else if(i && (i % GROUP_SIZE) == 0)
            putchar(' ');

        (test_bit(imap, i)) ? putchar('1') : putchar('0');
    }
    putchar('\n');

    free(imap);
}

void print_inode(int fd, int inode_number)
{
    INODE inode = get_inode(fd, inode_number);

    printf("\n********** INODE: %d **********\n", inode_number);
    printf("mode               =  %4x\n", inode.i_mode);
    printf("uid                =  %u\n" , inode.i_uid);
    printf("gid                =  %u\n" , inode.i_gid);
    printf("size               =  %u\n" , inode.i_size);
    printf("time               =  %s"   , ctime((time_t*)&inode.i_mtime));
    printf("links_count        =  %u\n" , inode.i_links_count);
    printf("i_block[0]         =  %u\n" , inode.i_block[0]);
}

void print_dir(int fd, int inode_number)
{
    INODE inode = get_inode(fd, inode_number);
    int block_size = get_block_size(fd);

    int i;
    printf("\n********** DIR OF INODE: %d **********\n", inode_number);
    for(i = 0; i < (inode.i_size / block_size); i++)
    {
        u8* block = get_block(fd, inode.i_block[i]);
        u8* cp = block; 
        DIR* dp = (DIR*)block;

        printf("i_block[%d]\n", i);
        printf("--------------------------------\n");
        printf(" inode  rec_len  name_len  name\n");
        printf("--------------------------------\n");

        while (cp < (block + block_size))
        {
            printf(" %3u    %5u      %3u     %s\n", 
                    dp->inode, dp->rec_len, dp->name_len, dp->name);
            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 
        putchar('\n');

        free(block);
    }
}
