#include <cmd.h>

int rm_child(MINODE *parent_mip, int child_ino)
{
    int i = 0;
    const int device = running->cwd->dev;
    const int block_size = get_block_size(device);
    INODE* parent_ip = &parent_mip->inode;

        DIR* dp = NULL; 
   //1. Search parent INODE's data block(s) for the entry of name

// For DIR inodes, assume that (the number of entries is small so that) only has
    // 12 DIRECT data blocks. Therefore, search only the direct blocks for name[0].
    for(i = 0; i < (parent_ip->i_size / block_size); i++)
    {
        if (parent_ip->i_block[i] == 0)
            break;

        u8* block = get_block(device, parent_ip->i_block[i]);
        u8* cp = block; 
        dp = (DIR*)block;

        while (cp < block + block_size)
        {

            if(dp->inode == child_ino)
            {
                free(block);
                break;
            }

            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 

        free(block);
    }
    //dp = child dir

    //2. Erase name entry from parent directory by

    //   if NOT first entry in block{
    //       move all entries AFTER this entry LEFT;
    //       add removed rec_len to the LAST entry of the block;
    //       no need to change parent's fileSize;

    //            | remove this entry   |
    //       -----------------------------------------------
    //       xxxxx|INO rlen nlen NAME   |yyy  |zzz         | 
    //       -----------------------------------------------

    //               becomes:
    //       -----------------------------------------------
    //       xxxxx|yyy |zzz (rec_len INC by rlen)          |
    //       -----------------------------------------------

    //   }
    
    //   if (first entry in a data block){
    //      deallocate the data block; modify parent's file size;

    //       -----------------------------------------------
    //       |INO Rlen Nlen NAME                           | 
    //       -----------------------------------------------
    //       
    //       Assume this is parent's i_block[i]:
    //       move parent's NONZERO blocks upward, i.e. 
    //            i_block[i+1] becomes i_block[i]
    //            etc.
    //       so that there is no HOLEs in parent's data block numbers
    //   }

    parent_mip->dirty = true;

    // Write the parent's data block back to disk;
    iput(parent_mip);

    return SUCCESS;
}

int my_rmdir(int argc, char* argv[])
{
    int i = 0;
    const int device = running->cwd->dev;
    const int uid = running->uid;

    if(argc < 2)
    {
        fprintf(stderr, "rmdir: missing operand\n");
        return FAILURE;
    }

    // rmdir each path given by user
    while(argv[++i])
    {
        char* path = argv[i];

        int ino = getino(device, path);
        MINODE* mip = iget(device, ino);
        INODE*   ip = &mip->inode;

        int parent_ino = 0;
        MINODE* parent_mip = NULL; 
        INODE*   parent_ip = NULL;

        // Verify user has permission to remove the directory
        if(uid != SUPER_USER && uid != ip->i_uid)
        {
            fprintf(stderr, "rmdir: you do not own '%s':"
                    " Permission denied\n", path);
            iput(mip);
            return FAILURE;
        }

        // Verify that it is a directory
        if(!S_ISDIR(ip->i_mode))
        {
            fprintf(stderr, "mkdir: cannot remove directory '%s':"
                    " Not a directory\n", path);
            iput(mip);
            return FAILURE;
        }

        // Verify that it is not busy
        if(mip->refCount > 1)
        {
            fprintf(stderr, "rmdir: failed to remove directory '%s':"
                    " Directory busy\n", path);
            iput(mip);
            return FAILURE;
        }

        // Verify that it is empty
        if(!isEmptyDir(mip))
        {
            fprintf(stderr, "rmdir: failed to remove directory '%s':"
                    " Directory not empty\n", path);
            iput(mip);
            return FAILURE;
        }

        // If removing multiple directories
        if(argc > 2)
            printf("rmdir: removing directory '%s'\n", path);

        // Get parent DIR's ino and Minode
        findino(mip, &ino, &parent_ino);
        parent_mip = iget(device, parent_ino); 
        parent_ip = &parent_mip->inode;

        // Deallocate its blocks
        for(i = 0; i < NUM_DIRECT_BLOCKS && ip->i_block[i] != 0; i++)
            bfree(device, ip->i_block[i]);

        // Deallocate its inode
        ifree(device, ino);

        // Write changes to deleted directory to disk and clear refCount
        iput(mip); 

        // Remove child's entry from parent directory by
        rm_child(parent_mip, ino); 

        // Update parent's info
        parent_ip->i_links_count--;
        parent_ip->i_atime = time(0L);
        parent_ip->i_mtime = time(0L);
        parent_mip->dirty = true;

        // Write parent changes to disk
        iput(parent_mip);
    }

    return SUCCESS;
}


