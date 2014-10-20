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

#define ROOT_INODE_NUMBER 2

int main(int argc, char *argv[])
{ 
    int fd;
    char *disk;

    if (argc > 1)
        disk = argv[1];
    else
        disk = "mydisk"; 

    if((fd = open(disk, O_RDWR)) < 0)
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
    SUPER* sp = get_super(fd);
    printf("ninodes=%d nblocks=%d nfreeInodes=%d nfreeBlocks=%d\n", 
            sp->s_inodes_count, sp->s_blocks_count, 
            sp->s_free_inodes_count, sp->s_free_blocks_count);
    free(sp);
    print_imap(fd);
    putchar('\n');

    int i;
    for(i = 0; i < 5; i++)
    {  
        int ino = ialloc(fd);
        printf("allocated ino = %d\n", ino);
    }
    putchar('\n');

    sp = get_super(fd);
    printf("ninodes=%d nblocks=%d nfreeInodes=%d nfreeBlocks=%d\n", 
            sp->s_inodes_count, sp->s_blocks_count, 
            sp->s_free_inodes_count, sp->s_free_blocks_count);
    free(sp);
    print_imap(fd);
    putchar('\n');

    return 0;
}
