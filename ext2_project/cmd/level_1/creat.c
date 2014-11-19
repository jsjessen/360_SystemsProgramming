#include <cmd.h>

int creat_file(MINODE* parent_mip, char* child_name);


// Create a new file
int my_creat(int argc, char* argv[])
{
    const int device = running->cwd->device;

    if(argc < 2)
    {
        fprintf(stderr, "creat: missing operand\n");
        return FAILURE;
    }

    // creat each filename given by user
    int i = 0;
    while(argv[++i])
    {
        char* path = argv[i];

        // From path, get path to parent and name of child
        char* parent_name = NULL;
        char* child_name  = NULL;
        parse_path(path, &parent_name, &child_name);

        // Get parent in memory
        int parent_ino = getino(device, parent_name);
        MINODE* parent_mip = iget(device, parent_ino);

        // Verify that parent exists
        if(!parent_mip)
        {
            fprintf(stderr, "creat: cannot create file '%s':"
                    " No such file or directory\n", path);
            goto clean_up;
        }
        // Verify that parent is a directory
        else if(!S_ISDIR(parent_mip->inode.i_mode))
        {
            fprintf(stderr, "creat: cannot create file '%s':"
                    " Not a directory\n", path);
            goto clean_up;
        }
        // Verify that child does not yet exist
        else if(getino(device, path) > 0)
        {
            fprintf(stderr, "creat: cannot create file '%s':"
                    " File exists\n", path);
            goto clean_up;
        }

        // If creating multiple files
        if(argc > 2)
            printf("creat: creating directory '%s'\n", path);

        INODE*  parent_ip  = &parent_mip->inode;

        // Make a file with the child's name in the parent directory
        creat_file(parent_mip, child_name);

        // Set parent's last time of access to current time
        parent_ip->i_atime = time(0L);

        // Parent memory-inode now has child in its directory
        // but the disk-inode does not, hence parent is dirty.
        parent_mip->dirty = true;

clean_up:
        // Move parent inode from memory to disk
        iput(parent_mip); 

        free(parent_name);
        free(child_name);
    }

    return SUCCESS;
}


// The inner workings of file creation
int creat_file(MINODE* parent_mip, char* child_name)
{
    const int device = running->cwd->device;
    const int block_size = get_block_size(device);

    // Allocate an inode for the new file 
    int ino = ialloc(device);

    // Create the inode in memory
    MINODE* mip = iget(device, ino);
    INODE*  ip  = &mip->inode;

    ip->i_mode = FILE_MODE;		     // Directory type and permissions
    ip->i_uid  = running->uid;	     // Owner uid 
    ip->i_gid  = running->gid;	     // Group Id
    ip->i_size = 0;	                 // Size in bytes 
    ip->i_links_count = 1;	         // Links to parent directory
    ip->i_atime = time(0L);          // Set last access to current time
    ip->i_ctime = time(0L);          // Set creation to current time
    ip->i_mtime = time(0L);          // Set last modified to current time
    ip->i_blocks = block_size / 512; // # of 512-byte blocks reserved for this inode 

    // Set all of the data blocks to 0
    for(int i = 0; i < NUM_DATA_BLOCKS; i++)
        ip->i_block[i] = 0;

    // Just made it, so of course it differs from disk
    mip->dirty = true; // mark minode dirty
    iput(mip);         // write INODE to disk

    // Make entry for me in parent directory
    enter_name(parent_mip, ino, child_name);

    return SUCCESS;
}
