// James Jessen
// 10918967   
// 
// CptS 360

#include "util.h"

bool isEmptyDir(MINODE *mip)
{
    int i = 0;
    const int device = mip->dev;
    const int block_size = get_block_size(device);
    INODE* ip = &mip->inode;

    //Check that dir is a directory
    if (!S_ISDIR(ip->i_mode))
    {
        fprintf(stderr, "Not a directory\n");
        return false;
    }

    if(ip->i_links_count > 2)
        return false;

    // For DIR inodes, assume that (the number of entries is small so that) only has
    // 12 DIRECT data blocks. Therefore, search only the direct blocks for name[0].
    for(i = 0; i < (ip->i_size / block_size); i++)
    {
        if (ip->i_block[i] == 0)
            break;

        u8* block = get_block(device, ip->i_block[i]);
        u8* cp = block; 
        DIR* dp = (DIR*)block;

        while (cp < block + block_size)
        {
            char name[256];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = 0;

            if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
            {
                free(block);
                return false;
            }

            cp += dp->rec_len;       // advance cp by rec_len BYTEs
            dp = (DIR*)cp;           // pull dp along to the next record
        } 

        free(block);
    }

    return true;    
}

int get_ideal_record_length(const int name_len)
{
    //     (4 * ((4 + 2 + 1 + 1 + name_len + 3) / 4))
    return (4 * ((      8       + name_len + 3) / 4)); // Ideal
}

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

        // Get parent's data block into a buffer
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
    parent_ip->i_block[i] = bno;

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

