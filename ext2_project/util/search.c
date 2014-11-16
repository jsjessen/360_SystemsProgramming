// James Jessen       
// 10918967          
// 
// CptS 360

#include "search.h"

char* find_name(MINODE *mip)
{
    int my_ino = 0;
    int parent_ino = 0;
    findino(mip, &my_ino, &parent_ino);

    MINODE* parent_mip = iget(running->cwd->device, parent_ino);

    char* my_name = NULL;
    findmyname(parent_mip, my_ino, &my_name);

    iput(parent_mip);
    return my_name;
}

int findmyname(MINODE *parent, int myino, char **my_name)
{
    int i = 0;;
    int device = 0;
    int block_size = 0;
    INODE* ip = NULL;

    if(myino == ROOT_INODE)
    {
        *my_name = (char*)malloc((strlen("/") + 1) * sizeof(char));
        strcpy(*my_name, "/");
        return SUCCESS;
    }

    if(!parent)
    {
        fprintf(stderr, "findmyname: null parent\n");
        return FAILURE;
    }

    device = parent->device;
    block_size = get_block_size(device);
    ip = &parent->inode;

    //Check that parent is a directory
    if (!S_ISDIR(ip->i_mode))
    {
        fprintf(stderr, "Not a directory\n");
        return FAILURE;
    }

    // For DIR inodes, assume that (the number of entries is small so that) only has
    // 12 DIRECT data blocks. Therefore, search only the direct blocks for name[0].
    for(i = 0; i < (ip->i_size / block_size); i++)
    {
        if (ip->i_block[i] == EMPTY || i >= NUM_DIRECT_BLOCKS)
            break;

        u8* block = get_block(device, ip->i_block[i]);
        u8* cp = block; 
        DIR* dp = (DIR*)block;

        while (cp < block + block_size)
        {
            if(dp->inode == myino)
            {
                *my_name = (char*)malloc((dp->name_len + 1) * sizeof(char));
                strncpy(*my_name, dp->name, dp->name_len);
                (*my_name)[dp->name_len] = 0;
                free(block);
                return SUCCESS;
            }
            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 
        free(block);
    }
    return FAILURE;   
}

int findino(MINODE *mip, int *myino, int *parent)
{
    int device = 0;
    INODE* ip = NULL;

    if(!mip)
    {
        fprintf(stderr, "findino: null mip\n");
        return FAILURE;
    }

    device = mip->device;
    ip = &mip->inode;

    //Check that mip is a directory
    if (!S_ISDIR(ip->i_mode))
    {
        fprintf(stderr, "Not a directory\n");
        return FAILURE;
    }

    u8* block = get_block(device, ip->i_block[0]);
    u8* cp = block; 
    DIR* dp = (DIR*)block;

    *myino = dp->inode;

    cp += dp->rec_len;       // advance cp by rec_len BYTEs
    dp = (DIR*)cp;           // pull dp along to the next record

    *parent = dp->inode;

    free(block);
    return SUCCESS;
}

// Searches through the device along pathname for target file 
// Returns the target file's inode number if found
int getino(int device, char* pathname)
{
    int ino = 0;
    char** name = NULL; 

    if(strcmp(pathname, "/") == 0)
        return ROOT_INODE;

    name = parse(pathname, "/");

    // Absolute or Relative?
    if(pathname[0] == '/')
        ino = search(root, name[0]);        // Absolute: start at root
    else
        ino = search(running->cwd, name[0]);// Relative: start at cwd

    // Continue search
    int i = 0;
    while(name[++i])
    {
        if((ino = search(iget(device, ino), name[i])) <= 0)
        {
            free_array(name);
            return -1;
        }
    }

    free_array(name);
    return ino;
}

// Searches through the directory for the target file
int search(MINODE *mip, char* target)
{
    int device = mip->device;
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
    // 12 DIRECT data blocks. Therefore, search only the direct blocks.
    for(int i = 0; i < (ip->i_size / block_size); i++)
    {
        if (ip->i_block[i] == EMPTY || i >= NUM_DIRECT_BLOCKS)
            break;

        u8* block = get_block(device, ip->i_block[i]);
        u8* cp = block; 
        DIR* dp = (DIR*)block;

        while (cp < block + block_size)
        {
            char name[MAX_FILE_NAME_LENGTH];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = 0;

            if (strcmp(name, target) == 0)
                target_ino = dp->inode;

            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 

        free(block);
    }

    return target_ino;    
}
