/* James Jessen             
 * 10918967                 
 *
 * CptS 360
 * PreLab #5
 */

#include "util.h"

int imap(int fd)
{
    char buf[BLK_SIZE];
    int i, j;

    get_block(fd, INODE_BITMAP, buf);

    printf("\n******************** Inodes Bitmap ********************");

    for (i = 0; i < (INODES_COUNT / 8) + 1; i++) 
    {
        if (i % 8 == 0)
            putchar('\n');
        else
            putchar(' ');

        for (j = 0; j < 8; j++)
        {
            if ( buf[i] & (1 << j) )
                putchar('1');
            else
                putchar('0');
        }
    }
}
