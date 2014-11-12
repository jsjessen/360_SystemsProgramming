#include <cmd.h>


int creat_file(MINODE* parent_mip, char* child_name)
{
    int i = 0;
    const int device = running->cwd->dev;
    const int block_size = get_block_size(device);

    // Allocate an inode for the new file 
    int ino = ialloc(device);

    // Create the inode in memory
    MINODE* mip = iget(device, ino);
    INODE* ip   = &mip->inode;

    ip->i_mode = FILE_MODE;		     // Directory type and permissions
    ip->i_uid  = running->uid;	     // Owner uid 
    ip->i_gid  = running->gid;	     // Group Id
    ip->i_size = 0;	                 // Size in bytes 
    ip->i_links_count = 1;	         // Parent directory
    ip->i_atime = time(0L);          // Set last access to current time
    ip->i_ctime = time(0L);          // Set creation to current time
    ip->i_mtime = time(0L);          // Set last modified to current time
    ip->i_blocks = block_size / 512; // # of 512-byte blocks reserved for this inode 

    // Set all of the data blocks to 0
    for(i = 0; i < 15; i++)
        ip->i_block[i] = 0;

    // Just made it, so of course it differs from disk
    mip->dirty = true; // mark minode dirty
    iput(mip);         // write INODE to disk

    // Make entry in parent directory for me
    enter_name(parent_mip, ino, child_name);

    return 1;
}

int my_creat(int argc, char* argv[])
{
    int i = 0;
    const int device = running->cwd->dev;

    if(argc < 2)
    {
        fprintf(stderr, "creat: missing operand\n");
        return 0;
    }

    // creat each filename given by user
    while(argv[++i])
    {
        char* path = argv[i];
        char* parent_name = NULL;
        char* child_name  = NULL;

        int parent_ino = 0;

        MINODE* parent_mip = NULL;
        INODE* parent_ip = NULL;

        // If creating multiple files
        if(argc > 2)
            printf("creat: creating directory '%s'\n", path);

        // From path, get path to parent and name of child
        parse_path(path, &parent_name, &child_name);

        // Get parent in memory
        parent_ino = getino(device, parent_name);
        parent_mip = iget(device, parent_ino);
        parent_ip  = &parent_mip->inode;

        // Verify that parent exists
        if(!parent_mip)
        {
            fprintf(stderr, "creat: cannot create file '%s':"
                    " No such file or directory\n", path);
            free(parent_name);
            free(child_name);
            return 0;
        }

        // Verify that parent is a directory
        if(!S_ISDIR(parent_ip->i_mode))
        {
            fprintf(stderr, "creat: cannot create file '%s':"
                    " Not a directory\n", path);
            free(parent_name);
            free(child_name);
            return 0;
        }

        // Verify that child does not yet exist
        if(getino(device, path) > 0)
        {
            fprintf(stderr, "creat: cannot create file '%s':"
                    " File exists\n", path);
            free(parent_name);
            free(child_name);
            return 0;
        }

        // Make a file with the child's name in the parent directory
        creat_file(parent_mip, child_name);

        // Set parent's last time of access to current time
        parent_ip->i_atime = time(0L);

        // Parent memory-inode now has child in its directory
        // but the disk-inode does not, hence parent is dirty.
        parent_mip->dirty = true;

        // Move parent inode from memory to disk (if no other references)
        iput(parent_mip);

        free(parent_name);
        free(child_name);
    }

    return 0;
}
