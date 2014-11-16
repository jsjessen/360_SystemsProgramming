#include <cmd.h>


int my_rmdir(int argc, char* argv[])
{
    const int uid = running->uid;
    const int device = running->cwd->device;

    if(argc < 2)
    {
        fprintf(stderr, "rmdir: missing operand\n");
        return FAILURE;
    }

    // rmdir each path given by user
    int i = 0;
    while(argv[++i])
    {
        char* path  = argv[i];
        int ino     = getino(device, path);
        MINODE* mip = iget(device, ino);

        // Verify file exists
        if(!mip)
        {
            goto clean_up;
            fprintf(stderr, "rmdir: failed to remove '%s':"
                    " No such file or directory\n", path);
        }
        // Verify user has permission to remove the directory
        else if(uid != SUPER_USER && uid != mip->inode.i_uid)
        {
            goto clean_up;
            fprintf(stderr, "rmdir: failed to remove '%s':"
                    " Permission denied\n", path);
        }
        // Verify that it is a directory
        else if(!S_ISDIR(mip->inode.i_mode))
        {
            goto clean_up;
            fprintf(stderr, "mkdir: failed to remove '%s':"
                    " Not a directory\n", path);
        }
        // Verify that it is not busy
        else if(mip->refCount > 1)
        {
            goto clean_up;
            fprintf(stderr, "rmdir: failed to remove directory '%s':"
                    " Directory busy\n", path);
        }
        // Verify that it is empty
        else if(!isEmptyDir(mip))
        {
            goto clean_up;
            fprintf(stderr, "rmdir: failed to remove directory '%s':"
                    " Directory not empty\n", path);
        }

        // If removing multiple directories, display
        if(argc > 2)
            printf("rmdir: removing directory '%s'\n", path);

        INODE* ip = &mip->inode;

        // Get parent DIR's ino and Minode
        int parent_ino = 0;
        findino(mip, &ino, &parent_ino);
        MINODE* parent_mip = iget(device, parent_ino); 
        INODE*   parent_ip = &parent_mip->inode;

        // Deallocate its blocks
        for(int b = 0; b < NUM_DIRECT_BLOCKS && ip->i_block[b] != 0; b++)
            bfree(device, ip->i_block[b]);

        // Deallocate its inode
        ifree(device, ino);

        // Remove child's entry from parent directory
        rm_child(parent_mip, ino); 

        // Update parent's info
        parent_ip->i_links_count--;
        parent_ip->i_atime = time(0L);
        parent_ip->i_mtime = time(0L);
        parent_mip->dirty = true;

        // Write parent changes to disk
        iput(parent_mip);

clean_up:
        // Write changes to deleted directory to disk and clear refCount
        iput(mip); 
    }

    return SUCCESS;
}
