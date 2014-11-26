// James Jessen       
// 10918967          
// 
// CptS 360
//
// --------------------------------------------------------------
// Displays the disk blocks of a file in an EXT2 file system.
// The showblock program runs as follows:
//
//        showblock   DEVICE    PATHNAME
//
// e.g.   showblock   mydisk    /a/b/c/d
//
// It locates the file named PATHNAME and prints the 
// disk blocks (direct, indirect, double-indirect) of the file.
// --------------------------------------------------------------

#include "util.h"
#include "print.h"
#include "parse.h"

int get_file_inode(int device, char* pathname)
{
    char** name = parse(pathname, "/");

    // Follow user provided pathname 
    int i = 0;
    while(name[i])
    {
        ino = search(ip, name[i]);
        if (ino == 0)
        {
            printf("\n Failure: '%s' does not exist!\n\n", pathname);
            exit(1);
        }

        ip = iget(ino);

        if (S_ISDIR(ip->i_mode) && name[i + 1])
        {
            printDivider('=');
            print(ip);
        }
        else
            fprintf(stderr, "%s is not a directory\n", name[i + 1]);

        i++
    }
    printDivider('-');
    printf(" Success: '%s' has been found!\n", name[n-1]);


}

int search(INODE *ip, char *name)
{
    int i;
    char *cp;
    DIR *dp;
    char buf[BLKSIZE];
    char temp[256];

    // For DIR inodes, assume that (the number of entries is small so that) only has
    // 12 DIRECT data blocks. Therefore, search only the direct blocks for name[0].
    for (i = 0; i < 12; i++)
    {
        //printf("iblock[%d] = %d\n", i, ip->i_block[i]);
        if (ip->i_block[i] == 0)
            break;

        get_block(fd, ip->i_block[i], buf);
        dp = (DIR *)buf;
        cp = buf;

        while (cp < buf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;

            if (strcmp(name, temp) == 0)
                return dp->inode;

            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }

    return 0;    
}

int main(int argc, char *argv[])
{ 
    char* dev;
    char* pathname;

    if (argc != 3)
    {
        printf("Usage:\n");
        printf("         showblock   DEVICE    PATHNAME\n");
        printf("  e.g.   showblock   mydisk    /a/b/c/d\n\n");
        exit(1);
    }

    dev = argv[1];
    pathname = argv[2];

    if((fd = open(dev, O_RDONLY)) < 0)
    {
        perror("open device");
        exit(1);
    }

    if(!isExt2(dev))
    {
        fprintf(stderr, "%s is not of the EXT2 format\n", dev);
        exit(1);
    }

    print_inode_block(fd, find_file_inode(fd, pathname));

    return 0;
}
