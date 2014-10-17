/* James Jessen 
 * 10918967    
 *
 * CptS 360
 * PreLab #5
 */

#include "util.h"

int inode(int fd)
{
    char buf[BLK_SIZE];

    get_block(fd, INODE_TABLE, buf);
    INODE* ip = (INODE *)buf + 1; //+1 to point to root

    printf("********** Root Inode Info **********\n");
    printf("inode_block  =  %u\n", 5);
    printf("mode         =  %x\n", ip->i_mode);
    printf("uid          =  %u\n", ip->i_uid);
    printf("gid          =  %u\n", ip->i_gid);
    printf("size         =  %u\n", ip->i_size);
    printf("time         =  %s"  , ctime((time_t *)&ip->i_mtime));
    printf("links_count  =  %u\n", ip->i_links_count);
    printf("i_block[0]   =  %u\n", ip->i_block[0]);
}
