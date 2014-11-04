// James Jessen       
// 10918967          
// 
// CptS 360

#include "search.h"


int findmyname(MINODE *parent, int myino, char *myname)
{
    // Given the parent DIR (MINODE pointer) and myino, this function finds 
    // the name string of myino in the parent's data block. This is the SAME
    // as SEARCH() by myino, then copy its name string into myname[ ].
    
    return 0;
}

int findino(MINODE *mip, int *myino, MINODE *parentino)
{
    // For a DIR Minode, extract the inumbers of . and .. 
    // Read in 0th data block. The inumbers are in the first two dir entries.
    
    return 0;
}

// Searches through the device along pathname for target file 
int search_fs(int device, char* pathname)
{
    int i = 0;
    char** name = parse(pathname, "/");
    int ino = ROOT_INODE;

    printf("\nSearching for %s\n", pathname);

    while(name[i])
    {
        putchar('\n');
        if(i)
            printf("Find %s in %s\n", name[i], name[i - 1]);
        else
            printf("Find %s in / \n", name[i]);

        if((ino = search_dir(device, ino, name[i])) < 0)
        {
            printf("\nCouldn't find: %s \n", name[i]);
            free_array(name);
            return -1;
        }
        print_divider('-');

        i++;
    }
    printf("\nFound!\n");

    free_array(name);
    return ino;
}

// Searches through the directory for the target file
int search_dir(int device, int dir, char* target)
{
    int i;
    int target_ino= 0;
    int block_size = get_block_size(device);
    INODE* ip = get_inode(device, dir);

    //Check that dir is a directory
    if (!S_ISDIR(ip->i_mode))
    {
        fprintf(stderr, "Not a directory\n");

        free(ip);
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

        print_divider('-');
        printf(" Inode #   Record Length   Name Length   Name \n");
        print_divider('-');

        while (cp < block + block_size)
        {
            char name[256];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = 0;
            printf(" %4d          %4d          %4d        %s",
                    dp->inode, dp->rec_len, dp->name_len, name);

            if (strcmp(name, target) == 0)
            {
                printf(" <%.*s here", 15 - dp->name_len, "---------------");
                target_ino = dp->inode;
            }
            putchar('\n');

            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 

        free(block);
    }

    free(ip);
    return target_ino;    
}
