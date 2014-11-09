// James Jessen       
// 10918967          
// 
// CptS 360

#include "search.h"


int findmyname(MINODE *parent, int myino, char *myname)
{
    int i;
    int device = parent->dev;
    int target_ino= 0;
    int block_size = get_block_size(device);
    INODE* ip = &parent->inode;

    //Check that parent is a directory
    if (!S_ISDIR(ip->i_mode))
    {
        fprintf(stderr, "Not a directory\n");
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

        //print_divider('-');
        //printf(" Inode #   Record Length   Name Length   Name \n");
        //print_divider('-');

        while (cp < block + block_size)
        {
            char name[256];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = 0;
           // printf(" %4d          %4d          %4d        %s",
           //         dp->inode, dp->rec_len, dp->name_len, name);

            if(dp->inode == myino)
            {
                //printf(" <%.*s here", 15 - dp->name_len, "---------------");
                myname = (char*)malloc(strlen(name) + 1);
                strcpy(myname, name);
            }
            putchar('\n');

            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 

        free(block);
    }

    return target_ino;   
}

int findino(MINODE *mip, int *myino, MINODE *parentino)
{
    int device = mip->dev;
    INODE* ip = &mip->inode;

    //Check that mip is a directory
    if (!S_ISDIR(ip->i_mode))
    {
        fprintf(stderr, "Not a directory\n");
        return -1;
    }

    u8* block = get_block(device, ip->i_block[0]);
    u8* cp = block; 
    DIR* dp = (DIR*)block;

    *myino = dp->inode;

    cp += dp->rec_len;       // advance cp by rec_len BYTEs
    dp = (DIR*)cp;           // pull dp along to the next record

    parentino =iget(device, dp->inode);

    free(block);

    return 0;
}

// Searches through the device along pathname for target file 
// Returns the target file's inode number if found
int getino(int device, char* pathname)
{
    int i = 0;
    char** name = parse(pathname, "/");
    int ino = ROOT_INODE;

    //printf("\nSearching for %s\n", pathname);

    while(name[i])
    {
        //putchar('\n');
        //if(i)
        //    printf("Find %s in %s\n", name[i], name[i - 1]);
        //else
        //    printf("Find %s in / \n", name[i]);

        if((ino = search(iget(device, ino), name[i])) <= 0)
        {
            //printf("no such file or directory\n");
            free_array(name);
            return -1;
        }
        //print_divider('-');

        i++;
    }
    //printf("\nFound!\n");

    free_array(name);
    return ino;
}

// Searches through the directory for the target file
int search(MINODE *mip, char* target)
{
    int i;
    int device = mip->dev;
    int target_ino= 0;
    int block_size = get_block_size(device);
    INODE* ip = &mip->inode;

    //Check that dir is a directory
    if (!S_ISDIR(ip->i_mode))
    {
        fprintf(stderr, "Not a directory\n");
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

        //print_divider('-');
        //printf(" Inode #   Record Length   Name Length   Name \n");
        //print_divider('-');

        while (cp < block + block_size)
        {
            char name[256];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = 0;
            //printf(" %4d          %4d          %4d        %s",
            //        dp->inode, dp->rec_len, dp->name_len, name);

            if (strcmp(name, target) == 0)
            {
                //printf(" <%.*s here", 15 - dp->name_len, "---------------");
                target_ino = dp->inode;
            }
            //putchar('\n');

            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 

        free(block);
    }

    return target_ino;    
}
