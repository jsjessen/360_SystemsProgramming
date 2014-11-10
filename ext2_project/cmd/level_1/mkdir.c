#include <cmd.h>


int enter_name(MINODE *parent_mip, int my_ino, char *my_name)
{

    // For each data block of parent DIR do { // assume: only 12 direct blocks
    //
    //     if (i_block[i]==0) BREAK;
    //
    //(1). get parent's data block into a buf[];
    //   
    //(2). EXT2 DIR entries: Each DIR entry has rec_len and name_len. Each entry's
    //     ideal length is   IEAL_LEN = 4*(8 + name_len + 3)/4). 
    //     All DIR entries in a data block have rec_len = IDEAL_LEN, except the last
    //     entry. The rec_len of the LAST entry is to the end of the block, which may
    //     be larger than its IDEAL_LEN.
    //
    //  --|-4---2----2--|----|---------|--------- rlen ->------------------------|
    //    |ino rlen nlen NAME|.........|ino rlen nlen|NAME                       |
    //  --------------------------------------------------------------------------
    //
    //(3). To enter a new entry of name with n_len, the needed length is
    //        need_length = 4*((8 + n_len + 3)/4)  // a multiple of 4
    //
    //(4). Step to the last entry in a data block (HOW?). 
    //
    //     Let remain = LAST entry's rec_len - its IDEAL_LENGTH;
    //
    //     if (remain >= need_length){
    //        enter the new entry as the LAST entry and trim the previous entry
    //        to its IDEAL_LENGTH; 
    //        goto (6) below.
    //     } 
    //
    //                          EXAMPLE:
    //
    //                                 |LAST entry 
    //  --|-4---2----2--|----|---------|--------- rlen ->------------------------|
    //    |ino rlen nlen NAME|.........|ino rlen nlen|NAME                       |
    //  --------------------------------------------------------------------------
    //                                                    |     NEW entry
    //  --|-4---2----2--|----|---------|----ideal_len-----|--- rlen=remain ------|
    //    |ino rlen nlen NAME|.........|ino rlen nlen|NAME|myino rlen nlen myname|
    //  --------------------------------------------------------------------------
    //
    //}
    //
    //(5).// Reach here means: NO space in existing data block(s)
    //
    //  Allocate a new data block; INC parent's isze by 1024;
    //  Enter new entry as the first entry in the new data block with rec_len=BLKSIZE.
    //
    //  |-------------------- rlen = BLKSIZE -------------------------------------
    //  |myino rlen nlen myname                                                  |
    //  --------------------------------------------------------------------------
    //
    //(6).Write data block to disk;

    return 0;
}

int make_dir(MINODE* parent_mip, char* child_name)
{
    int i = 0;
    int device = running->cwd->dev;

    u8 block[BLOCK_SIZE];
    u8* cp  = NULL;
    DIR* dp = NULL;

    // Allocate an inode and a disk block for the new directory
    int ino = ialloc(device);
    int bno = balloc(device);

    // Create the inode in memory
    MINODE* mip = iget(device, ino);
    INODE *ip   = &mip->inode;

    ip->i_mode = DIR_MODE;		// DIR type and permissions
    ip->i_uid  = running->uid;	// Owner uid 
    ip->i_gid  = running->gid;	// Group Id
    ip->i_size = BLOCK_SIZE;		// Size in bytes 
    ip->i_links_count = 2;	        // Links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);  // set to current time
    ip->i_blocks = BLOCK_SIZE / 512; // LINUX: Blocks count in 512-byte chunks 
    ip->i_block[0] = bno;             // new DIR has one data block   

    for(i = 1; i < 15; i++)
        ip->i_block[i] = 0;

    mip->dirty = true;               // mark minode dirty
    iput(mip);                    // write INODE to disk

    // Create data block for new DIR containing . and .. entries

    //   | entry .     | entry ..                                             |
    //   ----------------------------------------------------------------------
    //   |ino|12|1|.   |pino|1012|2|..                                        |
    //   ----------------------------------------------------------------------

    cp = block;
    dp = (DIR*)block;

    dp->inode = (u32)ino;
    dp->rec_len = (u16)(4 * ((4 + 2 + 1 + 1 + strlen(".") + 3) / 4)); // Ideal
    dp->name_len = (u8)strlen(".");
    dp->file_type = (u8)EXT2_FT_DIR;
    dp->name[0] = '.';

    cp += dp->rec_len;       // advance cp by rec_len BYTEs
    dp = (DIR*)cp;           // pull dp along to the next record

    dp->inode = (u32)parent_mip->ino;
    dp->rec_len = (u16)(BLOCK_SIZE - (cp - block)); // The rest of the block 

    // DEBUG
    printf(".. rec_len = %d : should be 1012\n", dp->rec_len);

    dp->name_len = (u8)strlen("..");
    dp->file_type = (u8)EXT2_FT_DIR;
    dp->name[0] = '.';
    dp->name[1] = '.';

    put_block(device, bno, block);

    // Make entry in parent directory for me
    enter_name(parent_mip, ino, child_name);

    return 1;
}

int my_mkdir(int argc, char* argv[])
{
    int i = 0;

    if(argc < 2)
    {
        fprintf(stderr, "mkdir: missing operand\n");
        return 0;
    }

    // mkdir each path given by user
    while(argv[++i])
    {
        int device = running->cwd->dev;

        char* parent_name = NULL;
        char* child_name  = NULL;

        int parent_ino = 0;
        int child_ino  = 0;

        MINODE* parent_mip = NULL;
        MINODE* child_mip  = NULL;

        printf("mkdir: creating directory '%s'\n", argv[i]);

        // give name values

        parent_ino = getino(device, parent_name);
        parent_mip = iget(device, parent_ino);

        // Verify that parent exists
        if(!parent_mip)
        {
            fprintf(stderr, "mkdir: cannot create directory \
                    '%s': No such file or directory\n", argv[i]);
            return 0;
        }

        // Verify that parent is a directory
        if(!S_ISDIR(parent_mip->inode.i_mode))
        {
            fprintf(stderr, "mkdir: cannot create directory \
                    '%s': Not a directory\n", argv[i]);
            return 0;
        }

        // Verify that child does not yet exist
        if(getino(device, argv[i]) > 0)
        {
            fprintf(stderr, "mkdir: cannot create directory \
                    '%s': File exists\n", argv[i]);
            return 0;
        }

        // Make a directory with the child's name in the parent's directory
        make_dir(parent_mip, child_name);

        // Child's '..' links to parent
        parent_mip->inode.i_links_count++;

        // Set parents last time of access to current time
        parent_mip->inode.i_atime = time(0L);

        // Parent memory-inode now has child in its directory
        // but the disk-inode does not, hence parent is dirty.
        parent_mip->dirty = true;

        iput(parent_mip);
    }

    return 0;
}
