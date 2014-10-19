/* James Jessen             
 * 10918967                
 *
 * CptS 360
 * PreLab #5
 */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "util_ext2.h"
#include "print_ext2.h"

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

    if(get_magic(fd) != 0xEF53)
    {
        printf("Not an ext2 file system\n");
        exit(1);
    }

    putchar('\n');
    print_super(fd);
    getchar();

    print_gd(fd); 
    getchar();

    print_bmap(fd);
    getchar();
    putchar('\n');

    print_imap(fd);
    getchar();
    putchar('\n');

    print_inode(fd, 2);
    getchar();

    print_dir(fd);

    return 0;
}
