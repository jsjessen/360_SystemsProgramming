/* James Jessen 
 * 10918967    
 *
 * CptS 360
 * PreLab #5
 */

#include "util.h"

int dir(int fd)
{
    char ibuf[BLK_SIZE];
    char dbuf[BLK_SIZE];
    int i, count;

    get_block(fd, INODE_TABLE, ibuf);
    INODE* ip = (INODE *)ibuf + 1; //+1 to point to root

    for(i = 0; i < 12; i++)
    {
        get_block(fd, ip->i_block[i], dbuf);

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
}
