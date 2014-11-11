#include <cmd.h>
#include <string.h>


int enter_name(MINODE *parent_mip, int my_ino, char *my_name)
{
    int i = 0;
    const int device = parent_mip->dev;
    const int block_size = get_block_size(device);

    INODE* parent_ip = &parent_mip->inode;

    u8* block = NULL;
    u8* cp    = NULL;
    DIR* dp   = NULL;

    int need_len = 0;
    int last_entry_ideal = 0;
    int remain = 0;

    int bno = 0;

    // For each data block of parent DIR do (assume: only 12 direct blocks)
    for(i = 0; i < (parent_ip->i_size / block_size); i++)
    {
        if(parent_ip->i_block[i] == 0) 
            break;

    bno = parent_ip->i_block[i];

        // Get parent's data block into a buf[];
        block = get_block(device, bno);
        cp = block; 
        dp = (DIR*)block;

        //     EXT2 DIR entries: Each DIR entry has rec_len and name_len. Each entry's
        //     ideal length is   IDEAL_LEN = 4*(8 + name_len + 3)/4). 
        //     All DIR entries in a data block have rec_len = IDEAL_LEN, except the last
        //     entry. The rec_len of the LAST entry is to the end of the block, which may
        //     be larger than its IDEAL_LEN.
        //  --|-4---2----2--|----|---------|--------- rlen ->------------------------|
        //    |ino rlen nlen NAME|.........|ino rlen nlen|NAME                       |
        //  --------------------------------------------------------------------------
        //
        //      To enter a new entry of name with n_len, the needed length is
        //      need_length = 4*((8 + n_len + 3)/4)  // a multiple of 4

        need_len = get_ideal_record_length(strlen(my_name));

        // Step to the last entry in a data block
        while(cp + dp->rec_len < block + block_size)
        {
            cp += dp->rec_len; // advance cp by rec_len bytes
            dp = (DIR*)cp;     // pull dp along to the next record
        } 

        //     Let remain = LAST entry's rec_len - its IDEAL_LENGTH;
        last_entry_ideal = get_ideal_record_length(dp->name_len);
        remain = dp->rec_len - last_entry_ideal;

        if(remain >= need_len)
        {
            //                                 |LAST entry 
            //  --|-4---2----2--|----|---------|--------- rlen ->------------------------|
            //    |ino rlen nlen NAME|.........|ino rlen nlen|NAME                       |
            //  --------------------------------------------------------------------------
            //                                                    |     NEW entry
            //  --|-4---2----2--|----|---------|----ideal_len-----|--- rlen=remain ------|
            //    |ino rlen nlen NAME|.........|ino rlen nlen|NAME|myino rlen nlen myname|
            //  --------------------------------------------------------------------------

            // Trim last entry down to idea length, making room for new entry
            dp->rec_len = last_entry_ideal;

            cp += dp->rec_len; // advance cp by rec_len bytes
            dp = (DIR*)cp;     // pull dp along to the next record

            // New entry should take up the rest of the block
            dp->inode = (u32)my_ino;
            dp->rec_len = (u16)(block_size - (cp - block)); 
            dp->name_len = (u8)strlen(my_name);
            dp->file_type = (u8)EXT2_FT_DIR;
            strcpy(dp->name, my_name);

            // Write data block to disk;
            put_block(device, bno, block);
            return 1;
        } 
    }
    // Reach here means: NO space in existing data block(s)

    //  Allocate a new data block
    bno = balloc(device);

    parent_ip->i_size += block_size;
    parent_mip->dirty = true;

    //  Enter new entry as the first entry in the new data block with rec_len=BLKSIZE.
    //
    //  |-------------------- rlen = BLKSIZE -------------------------------------
    //  |myino rlen nlen myname                                                  |
    //  --------------------------------------------------------------------------

    // Get parent's data block into a buf[]
    block = get_block(device, bno); 
    cp = block; 
    dp = (DIR*)block;

    dp->inode = (u32)my_ino;
    dp->rec_len = (u16)block_size;
    dp->name_len = (u8)strlen(my_name);
    dp->file_type = (u8)EXT2_FT_DIR;
    strcpy(dp->name, my_name);

    // Write data block to disk
    put_block(device, bno, block);
    return 1;
}

int make_dir(MINODE* parent_mip, char* child_name)
{
    int i = 0;
    const int device = running->cwd->dev;
    const int block_size = get_block_size(device);

    u8* block;
    u8* cp  = NULL;
    DIR* dp = NULL;

    // Allocate an inode and a disk block for the new directory
    int ino = ialloc(device);
    int bno = balloc(device);

    // Create the inode in memory
    MINODE* mip = iget(device, ino);
    INODE* ip   = &mip->inode;

    ip->i_mode = DIR_MODE;		     // Directory type and permissions
    ip->i_uid  = running->uid;	     // Owner uid 
    ip->i_gid  = running->gid;	     // Group Id
    ip->i_size = block_size;	     // Size in bytes 
    ip->i_links_count = 2;	         // . and ..
    ip->i_atime = time(0L);          // Set last access to current time
    ip->i_ctime = time(0L);          // Set creation to current time
    ip->i_mtime = time(0L);          // Set last modified to current time
    ip->i_blocks = block_size / 512; // # of 512-byte blocks reserved for this inode 
    ip->i_block[0] = bno;            // new DIR has one data block   

    // Set all of the other blocks to 0
    for(i = 1; i < 15; i++)
        ip->i_block[i] = 0;

    // Just made it, so of course it differs from disk
    mip->dirty = true; // mark minode dirty
    iput(mip);         // write INODE to disk

    // Create data block for new DIR containing . and .. entries

    //   | entry .     | entry ..                                             |
    //   ----------------------------------------------------------------------
    //   |ino|12|1|.   |pino|1012|2|..                                        |
    //   ----------------------------------------------------------------------

    block = get_block(device, bno); 
    cp = block;
    dp = (DIR*)block;

    dp->inode = (u32)ino;
    dp->rec_len = (u16)get_ideal_record_length(strlen("."));
    dp->name_len = (u8)strlen(".");
    dp->file_type = (u8)EXT2_FT_DIR;
    dp->name[0] = '.';

    cp += dp->rec_len;       // advance cp by rec_len BYTEs
    dp = (DIR*)cp;           // pull dp along to the next record

    dp->inode = (u32)parent_mip->ino;
    dp->rec_len = (u16)(block_size - (cp - block)); // The rest of the block 

    dp->name_len = (u8)strlen("..");
    dp->file_type = (u8)EXT2_FT_DIR;
    dp->name[0] = '.';
    dp->name[1] = '.';

    put_block(device, bno, block);

    // Make entry in parent directory for me
    enter_name(parent_mip, ino, child_name);

    return 1;
}

// Command accessed via main, big picture
int my_mkdir(int argc, char* argv[])
{
    int i = 0;
    const int device = running->cwd->dev;

    if(argc < 2)
    {
        fprintf(stderr, "mkdir: missing operand\n");
        return 0;
    }

    // mkdir each path given by user
    while(argv[++i])
    {
        char* path = argv[i];
        char* parent_name = NULL;
        char* child_name  = NULL;

        int parent_ino = 0;

        MINODE* parent_mip = NULL;
        INODE* parent_ip = NULL;

        printf("mkdir: creating directory '%s'\n", path);

        // From path, get path to parent and name of child
        parse_path(path, &parent_name, &child_name);

        // Get parent in memory
        parent_ino = getino(device, parent_name);
        parent_mip = iget(device, parent_ino);

        parent_ip = &parent_mip->inode;

        // Verify that parent exists
        if(!parent_mip)
        {
            fprintf(stderr, "mkdir: cannot create directory \
                    '%s': No such file or directory\n", path);
            return 0;
        }

        // Verify that parent is a directory
        if(!S_ISDIR(parent_ip->i_mode))
        {
            fprintf(stderr, "mkdir: cannot create directory \
                    '%s': Not a directory\n", path);
            return 0;
        }

        // Verify that child does not yet exist
        if(getino(device, path) > 0)
        {
            fprintf(stderr, "mkdir: cannot create directory \
                    '%s': File exists\n", path);
            return 0;
        }

        // Make a directory with the child's name in the parent directory
        make_dir(parent_mip, child_name);

        // Child's '..' links to parent, so increment link count
        parent_ip->i_links_count++;

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
