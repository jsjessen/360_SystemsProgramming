// James Jessen       
// 10918967          
// 
// CptS 360

#include "search.h"

static const int ROOT_INODE = 2;

// Searches through the device along pathname for target file 
int search_fs(int device, char* pathname)
{
    char** name = parse(pathname, "/");
    int ino = search_dir(device, ROOT_INODE, name[0]);

    int i = 1;
    while(name[i])
    {
        if((ino = search_dir(device, ino, name[i])) < 0)
        {
            printf("\n Failed to find: %s \n\n", pathname);
            free_arr(name);
            return -1;
        }

        i++; // Next filename

        if (name[i])
        {
            print_divider('=');
            print_inode(device, ino);
        }
    }
    printDivider('-');
    printf(" Success: '%s' has been found!\n", name[i - 1]);

    free_arr(name);
    return ino;
}

// Searches through the directory for the target file
int search_dir(int device, int dir, char* target)
{
    int i;
    int block_size = get_block_size(device);
    INODE* ip = get_inode(device, dir);

    //Check that dir is a directory
    if (!S_ISDIR(ip->i_mode))
    {
        fprintf(stderr, "%s is not a directory\n", name[i + 1]);
        return -1;
    }

    // For DIR inodes, assume that (the number of entries is small so that) only has
    // 12 DIRECT data blocks. Therefore, search only the direct blocks for name[0].
    for(i = 0; i < (ip->i_size / block_size); i++)
    {
        if (ip->i_block[i] == 0)
            break;

        u8* block = get_block(device, ip->i_block[i]);
        u8* cp = block; 
        DIR* dp = (DIR*)block;

        printf("i_block[%d]\n", i);
        print_divider('-');
        printf(" inode  rec_len  name_len  name\n");
        print_divider('-');

        while (cp < block + block_size)
        {
            char name[256];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = 0;
            printf(" %4d          %4d          %4d        %s\n",
                    dp->inode, dp->rec_len, dp->name_len, name);

            if (strcmp(name, target) == 0)
                return dp->inode;

            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 
        putchar('\n');
        free(block);
    }
    return -1;    
}
