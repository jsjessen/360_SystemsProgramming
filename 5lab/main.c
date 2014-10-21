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
#include "search.h"
#include "print.h"

int main(int argc, char *argv[])
{ 
    char* device;
    char* pathname;

    if (argc != 3)
    {
        printf("Usage:\n");
        printf("         showblock   DEVICE    PATHNAME\n");
        printf("  e.g.   showblock   mydisk    /a/b/c/d\n\n");
        exit(1);
    }

    device = argv[1];
    pathname = argv[2];

    if((fd = open(device, O_RDONLY)) < 0)
    {
        perror("open device");
        exit(1);
    }

    if(!isExt2(device))
    {
        fprintf(stderr, "%s does not use the ext2 filesystem\n", device);
        exit(1);
    }

    print_file_blocks(fd, search_fs(fd, pathname));

    return 0;
}
