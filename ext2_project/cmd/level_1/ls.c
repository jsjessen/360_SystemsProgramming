#include <cmd.h>

void list_dir(MINODE* mip);
void list_file(MINODE* mip, char* name);


// ls     => ls <cwd>
// ls dir => ls <cwd>/dir
// ls a/b/c 
// ls a/b/c e/f/g /h/i/j
int my_ls(int argc, char* argv[])
{
    const int device = running->cwd->device;

    // If given no path, ls cwd
    if(argc < 2)
    {
        list_dir(running->cwd);
        return SUCCESS;
    }

    // ls each path given by user
    int i = 0;
    while(argv[++i])
    {
        int ino = getino(device, argv[i]);
        MINODE* mip = iget(device, ino);

        if(!mip)
        {
            printf("ls: cannot access '%s':"
                    " No such file or directory\n", argv[i]);
            goto clean_up;
        }

        // If printing multiple lists label each one
        if(argc > 2)
            printf("%s:\n", argv[i]);

        if(S_ISDIR(mip->inode.i_mode))
            list_dir(mip);
        else
            list_file(mip, argv[i]);

clean_up:
        // Move parent inode from memory to disk
        iput(mip);
    }

    return SUCCESS;
}


// Prints information about a given inode
// permissions  links  gid  uid  size  date_last_modified  name
void list_file(MINODE* mip, char* name)
{
    if(!mip)
    {
        fprintf(stderr, "list_file: Null pointer to memory inode\n");
        return;
    }

    INODE *ip = &mip->inode;

    u16 mode   = ip->i_mode;
    u16 links  = ip->i_links_count;
    u16 uid    = ip->i_uid;
    u16 gid    = ip->i_gid;
    u32 size   = ip->i_size;

    char* time = ctime((time_t*)&ip->i_mtime);
    time[strlen(time) - 1] = 0; // remove newline suffix

    static const char* Permissions = "rwxrwxrwx";

    // Type 
    // The leading 4 bits of mode (2 bytes/16 bits) indicate type
    // 0xF000 = 1111 0000 0000 0000
    switch(mode & 0xF000) 
    {
        case 0x8000:  putchar('-');     break; // 0x8 = 1000
        case 0x4000:  putchar('d');     break; // 0x4 = 0100
        case 0xA000:  putchar('l');     break; // oxA = 1010
        default:      putchar('?');     break;
    }

    // Permissions
    for(int i = 0; i < strlen(Permissions); i++)
        putchar(mode & (1 << (strlen(Permissions) - 1 - i)) ? Permissions[i] : '-');

    // Everything else
    printf("%4hu %4hu %4hu %8u %26s  %s", 
            links, gid, uid, size, time, name);

    // Trace link
    if(S_ISLNK(ip->i_mode))
        printf(" -> %s", (char*)ip->i_block);
}

// Displays each directory entry
// Assumes the given minode exists and is a directory
void list_dir(MINODE* mip)
{
    const int device = mip->device;
    const int block_size = get_block_size(device);

    INODE* ip = &mip->inode;

    // For DIR inodes, assume that (the number of entries is small so that) only has
    // 12 DIRECT data blocks. Therefore, search only the direct blocks.
    for(int i = 0; i < (ip->i_size / block_size); i++)
    {
        if (ip->i_block[i] == EMPTY || i >= NUM_DIRECT_BLOCKS)
            break;

        u8* block = get_block(device, ip->i_block[i]);
        u8* cp = block; 
        DIR* dp = (DIR*)block;

        // While within the block
        while (cp < block + block_size)
        {
            char name[MAX_FILE_NAME_LENGTH];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = '\0';

            MINODE* cip = iget(device, dp->inode);
            list_file(cip, name);
            putchar('\n');
            iput(cip);

            cp += dp->rec_len; // advance cp by rec_len bytes 
            dp = (DIR*)cp;     // pull dp along to the next record
        } 

        free(block);
    }
}
