#include <cmd.h>

int make_dir(MINODE* parent_mip, char* child_name);


// Command accessed via main, big picture
int my_mkdir(int argc, char* argv[])
{
    const int device = running->cwd->device;

    if(argc < 2)
    {
        fprintf(stderr, "mkdir: missing operand\n");
        return FAILURE;
    }

    // mkdir each path given by user
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
        INODE*   parent_ip = &parent_mip->inode;

        // Verify that parent exists
        if(!parent_mip)
        {
            goto clean_up;
            fprintf(stderr, "mkdir: cannot create directory '%s':"
                    " No such file or directory\n", path);
        }
        // Verify that parent is a directory
        else if(!S_ISDIR(parent_ip->i_mode))
        {
            goto clean_up;
            fprintf(stderr, "mkdir: cannot create directory '%s':"
                    " Not a directory\n", path);
        }    
        // Verify that child does not yet exist
        else if(getino(device, path) > 0)
        {
            goto clean_up;
            fprintf(stderr, "mkdir: cannot create directory '%s':"
                    " File exists\n", path);
        }

        // If creating multiple directories
        if(argc > 2)
            printf("mkdir: creating directory '%s'\n", path);

        // Make a directory with the child's name in the parent directory
        make_dir(parent_mip, child_name);

        // Child's '..' links to parent, so increment link count
        parent_ip->i_links_count++;

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


// The inner workings of directory creation
int make_dir(MINODE* parent_mip, char* child_name)
{
    const int device = running->cwd->device;
    const int block_size = get_block_size(device);

    // Allocate an inode and a disk block for the new directory
    int ino = ialloc(device);
    int bno = balloc(device);

    // Create the inode in memory
    MINODE* mip = iget(device, ino);
    INODE*   ip = &mip->inode;

    ip->i_mode        = DIR_MODE;		  // Directory type and permissions
    ip->i_uid         = running->uid;	  // Owner uid 
    ip->i_gid         = running->gid;	  // Group Id
    ip->i_size        = block_size;	      // Size in bytes 
    ip->i_links_count = 2;	              // . and ..
    ip->i_atime       = time(0L);         // Set last access to current time
    ip->i_ctime       = time(0L);         // Set creation to current time
    ip->i_mtime       = time(0L);         // Set last modified to current time
    ip->i_blocks      = block_size / 512; // # of 512-byte blocks reserved for this inode 
    ip->i_block[0]    = bno;              // Has 1 data block for . and .. dir entries

    // Set all of the other data blocks to 0
    for(int i = 1; i < NUM_DATA_BLOCKS; i++)
        ip->i_block[i] = 0;

    // Just made it, so of course it differs from disk
    mip->dirty = true; // mark minode dirty
    iput(mip);         // write INODE to disk

    // Create data block for new DIR containing . and .. entries

    //   | entry .     | entry ..                                             |
    //   ----------------------------------------------------------------------
    //   |ino|12|1|.   |pino|1012|2|..                                        |
    //   ----------------------------------------------------------------------

    u8* block = get_block(device, bno); 
    u8*  cp   = block;
    DIR* dp   = (DIR*)block;

    dp->inode     = (u32)ino;
    dp->rec_len   = (u16)get_ideal_record_length(strlen("."));
    dp->name_len  = (u8)strlen(".");
    dp->file_type = (u8)EXT2_FT_DIR;
    dp->name[0]   = '.';

    cp += dp->rec_len;       // advance cp by rec_len BYTEs
    dp = (DIR*)cp;           // pull dp along to the next record

    dp->inode     = (u32)parent_mip->ino;
    dp->rec_len   = (u16)(block_size - (cp - block)); // The rest of the block 

    dp->name_len  = (u8)strlen("..");
    dp->file_type = (u8)EXT2_FT_DIR;
    dp->name[0]   = '.';
    dp->name[1]   = '.';

    put_block(device, bno, block);

    // Make entry in parent directory for me
    enter_name(parent_mip, ino, child_name);

    return SUCCESS;
}
