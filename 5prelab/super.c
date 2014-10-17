/* James Jessen             
 * 10918967                  
 *
 * CptS 360
 * PreLab #5
 */

#include "util.h"

int super(int fd)
{
    char buf[BLK_SIZE];

    get_block(fd, 1, buf);
    SUPER* sp = (SUPER *)buf;

    MAGIC = sp->s_magic;
    INODES_COUNT = sp->s_inodes_count;
    BLOCKS_COUNT = sp->s_blocks_count;

    printf("********** Super Block **********\n");
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
    printf("mtime              =  %s"  , ctime((time_t *)&sp->s_mtime));
    printf("inode_size         =  %u\n", sp->s_inode_size);
}
