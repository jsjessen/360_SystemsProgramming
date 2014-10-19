/* James Jessen             
 * 10918967                  
 *
 * CptS 360
 */

#include "print_ext2.h"

static const int ROOT_INODE = 2;

void print_super(int fd)
{
    SUPER* sp = get_super(fd); 

    printf("********** SUPER **********\n");
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

    printf("********** GROUP DESCRIPTOR **********\n");
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
    printf("********** BLOCK BITMAP **********\n");
    for(i = 0; i < blocks_count; i++)
    {
        if(i && (i % (GROUPS_PER_LINE * GROUP_SIZE)) == 0)
            putchar('\n');
        else if(i && (i % GROUP_SIZE) == 0)
            putchar(' ');

        (test_bit(bmap, i)) ? putchar('1') : putchar('0');
    }

    free(bmap);
}

void print_imap(int fd)
{
    u8* imap = get_inode_bitmap(fd);
    int inode_count = get_inodes_count(fd);

    int i;
    printf("********** INODE BITMAP **********\n");
    for(i = 0; i < inode_count; i++)
    {
        if(i && (i % (GROUPS_PER_LINE * GROUP_SIZE)) == 0)
            putchar('\n');
        else if(i && (i % GROUP_SIZE) == 0)
            putchar(' ');

        (test_bit(imap, i)) ? putchar('1') : putchar('0');
    }

    free(imap);
}

void print_inode(int fd, int inode)
{
    INODE* ip = get_inode(fd, inode);

    printf("********** INODE TABLE **********\n");
    printf("mode               =  %4x\n", ip->i_mode);
    printf("uid                =  %u\n" , ip->i_uid);
    printf("gid                =  %u\n" , ip->i_gid);
    printf("size               =  %u\n" , ip->i_size);
    printf("time               =  %s"   , ctime((time_t*)&ip->i_mtime));
    printf("links_count        =  %u\n" , ip->i_links_count);
    printf("i_block[0]         =  %u\n" , ip->i_block[0]);

    free(ip - (inode - 1));
}

void print_dir(int fd)
{
}
/*
//char ibuf[BLK_SIZE];
//char dbuf[BLK_SIZE];
int i, count;

//get_block(fd, INODE_TABLE, ibuf);
//INODE* ip = (INODE *)ibuf + 1; //+1 to point to root

for(i = 0; i < 12; i++)
{
//get_block(fd, ip->i_block[i], dbuf);
dbuf = get_block(fd, ip->i_block[i]);

if(!dbuf || dbuf[0] == 0)
break;

DIR* dp = (DIR *)dbuf;
char* cp = dbuf;

printf("****************************************\n");
printf("i_block[%d] = %d\n", i, ip->i_block[i]);

while (cp < (dbuf + BLK_SIZE))
{
printf(" %3u ", dp->inode);
printf(" %5u ", dp->rec_len);
printf(" %3u ", dp->name_len);
printf(" %s ", dp->name);
printf("\n");

cp += dp->rec_len;       // advance cp by rec_len BYTEs
dp = (DIR *)cp;          // pull dp along to the next record
}
}

free(ip);
}
*/
