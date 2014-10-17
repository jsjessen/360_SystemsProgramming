/* James Jessen             
 * 10918967                
 *
 * CptS 360
 * PreLab #5
 */

#include "util.h"

int main(int argc, char *argv[])
{ 
    int fd;
    char *disk;

    if (argc > 1)
        disk = argv[1];
    else
        disk = "mydisk"; 
    if((fd = open(disk, O_RDONLY)) < 0)
    {
        perror("Open disk");
        exit(1);
    }

    putchar('\n');

    super(fd);
    getchar();

    gd(fd); 
    getchar();

    bmap(fd);
    getchar();

    imap(fd);
    getchar();

    inode(fd);
    getchar();

    dir(fd);

    return 0;
}
