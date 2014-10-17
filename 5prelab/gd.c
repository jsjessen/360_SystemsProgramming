/* James Jessen 
 * 10918967    
 *
 * CptS 360
 * PreLab #5
 */

#include "util.h"

int gd(int fd)
{
    char buf[BLK_SIZE];

    get_block(fd, 2, buf);
    GD* gp = (GD *)buf;

    BLOCK_BITMAP = gp->bg_block_bitmap;
    INODE_BITMAP = gp->bg_inode_bitmap;
    INODE_TABLE = gp->bg_inode_table;

    printf("********** Group Descriptor Block **********\n");
    printf("block_bitmap       =  %u\n", gp->bg_block_bitmap);
    printf("inode_bitmap       =  %u\n", gp->bg_inode_bitmap);
    printf("inode_table        =  %u\n", gp->bg_inode_table);
    printf("free_blocks_count  =  %u\n", gp->bg_free_blocks_count);
    printf("free_inodes_count  =  %u\n", gp->bg_free_inodes_count);
    printf("used_dirs_count    =  %u\n", gp->bg_used_dirs_count);
}
