#include <file.h>

//     running
//       |                                                  
//       |                                                    ||****************** 
//     PROC[ ]              OFT[ ]              MINODE[ ]     ||      Disk dev
//   ===========    |---> ===========    |--> ============    || ===============
//   |ProcPtr  |    |     |mode     |    |    |  INODE   |    || |      INODE   
//   |pid, ppid|    |     |refCount |    |    | -------  |    || =============== 
//   |uid      |    |     |minodePtr|---->    | dev,ino  |    || 
//   |cwd      |    |     |offset   |         | refCount |    ||******************
//   |         |    |     ====|======         | dirty    |
//   |  fd[10] |    |         |               | mounted  |         
//   | ------  |    |         |               ============
// 0 | ------->|--->|         |
//   | ------  |              |   
// 1 | ------  |              |
//   | ------  |             --------------------------------
// 2 | ------  |             |0123456.............
//   | ------  |             --------------------------------    
//   ===========        logical view of file: a sequence of bytes

int open_file(char* pathname, int mode)
{
    int device = 0;
    if(pathname[0] == '/')
        device = root->device;
    else
        device = running->cwd->device;

    int ino = getino(device, pathname);
    MINODE* mip = iget(device, ino);
    INODE* ip = &mip->inode; 

    // Verify it is a regular file
    if(!S_ISREG(ip->i_mode))
    {
        fprintf(stderr, "open: failed to open'%s':"
                " Not a regular file\n", pathname);
        goto clean_up;
    }

    // User has 'all user' permissions
    // --- --- rwx
    int shift = 0;

    // User has 'owner' permissions
    if(running->uid == ip->i_uid)
    {
        // rwx --- ---
        shift = 6;
    }
    // User has 'group' permissions
    else if(running->gid == ip->i_gid)
    {
        // --- rwx ---
        shift = 3;
    }

    // binary: 7 = 111
    u16 mask = mode & (7 << shift);

    switch(mode)
    {
        case RD:
            // binary: 4 = 100
            if((4 << shift) & mask == 0)
                goto denied;

        case WR:
            // binary: 2 = 010
            if((2 << shift) & mask == 0)
                goto denied;

        case RW:
            // binary: 6 = 110 
            if((6 << shift) & mask == 0)
                goto denied;

        case APPEND:
            // binary: 2 = 010
            if((2 << shift) & mask == 0)
                goto denied;

        default:
            fprintf(stderr, "open: failed to open '%s':"
                    " Unknown mode\n", pathname);
denied:
            fprintf(stderr, "open: '%s':"
                    " Permission denied\n", pathname);
            goto clean_up;
    }

    OPEN_FILE* fp = NULL;

    for(int i = 0; i < NOFT; i++)
    {
        // Check if the file is already open
        // and verify it is open with a compatible mode
        if(mode != RD 
                && OpenFileTable[i].mip->ino == mip->ino
                && OpenFileTable[i].mode != RD)
        {
            fprintf(stderr, "open: failed to open '%s':"
                    " File in use\n", pathname);
            goto clean_up;
        }

        // Make entry in first not in use OpenFileTable entry
        if(OpenFileTable[i].refCount == 0)
        {
            fp = &OpenFileTable[i];
            fp->mode = mode;
            fp->refCount = 1;
            fp->mip = mip;
            break;
        }

        // No more available space in the OpenFileTable
        if(i == NOFT - 1)
        {
            fprintf(stderr, "open: failed to open '%s':"
                    " Too many files open\n", pathname);
            goto clean_up;
        }
    }

    // Set offset according to open mode
    switch(mode)
    {
        case RD: 
            fp->offset = 0; 
            break;

            // WR: truncate file to 0 size
        case WR: 
            my_truncate(mip); 
            fp->offset = 0;
            break;

            // RW does NOT truncate file
        case RW: 
            fp->offset = 0; 
            break;

            // APPEND mode
        case APPEND: 
            fp->offset = ip->i_size;  
            break;

        default: printf("invalid mode\n");
                 return FAILURE;
    }

    int fd = 0;
    for(fd; fd < NFD; fd++)
    {
        if(running->fd[fd] == NULL)
        {
            running->fd[fd] = fp;
            break;
        }

        if(fd == NFD - 1)
        {
            fprintf(stderr, "open: failed to open '%s':"
                    " Process has reached file limit\n", pathname);
            goto clean_up;
        }
    }

    // Update inode's time field
    ip->i_atime = time(0L);
    mip->dirty = true;

clean_up:
    iput(mip);

    return fd;
}

int close_file(int fd)
{
    // Verify fd is within range.
    if(fd < 0 || fd >= NFD)
    {
        fprintf(stderr, "close: failed to close %d:"
                " File descriptor out of range\n", fd);
        return FAILURE;
    }

    // Verify running->fd[fd] is pointing at a OFT entry
    for(int i = 0; i < NOFT; i++)
    {
        if(OpenFileTable[i].mip == running->fd[fd]->mip)
            break;

        if(i == NOFT - 1)
        {
            fprintf(stderr, "close: failed to close %d:"
                    " File not in OpenFileTable\n", fd);
            return FAILURE;
        }
    }

    OPEN_FILE* fp = running->fd[fd];
    running->fd[fd] = 0;
    fp->refCount--;

    if(fp->refCount > 0) 
        return SUCCESS; 

    // last user of this OFT entry ==> dispose of the Minode[]
    iput(fp->mip);

    return SUCCESS; 
}

//                        |<------- avail ------>|
//     -------------------|-----------------------
//     |    |    | ...  |lbk  |   |  ...| .......|
//     -------------------|---|------------------|-
// lbk   0    1 .....     |rem|                   |
//                      start                   fsize  

// It tries to read nbytes from fd to buf[ ], and returns the 
// actual number of bytes read.
int my_read(int fd, char* buf, int nbytes)
{
    OPEN_FILE* fp = running->fd[fd]; // Open file table pointer

    // Verify fd is open for read
    if(fp->mode != RD && fp->mode != RW)
    {
        fprintf(stderr, "read: failed: file descriptor %d is not open for read\n", fd);
        return FAILURE;
    }

    MINODE* mip = fp->mip;
    INODE* ip = &mip->inode;

    const int device = mip->device;
    const int block_size = get_block_size(device);
    const int file_size = ip->i_size;

    int count = 0;
    int avail = file_size - fp->offset; // number of bytes still available in file.
    char *cq = buf;   // cq points at buf[ ]

    while(nbytes && avail)
    {
        //Compute LOGICAL BLOCK number logical_block and start_byte in that block from offset;
        int logical_block = fp->offset / block_size;
        int start_byte   = fp->offset % block_size;

        // get the data block into read_buf[block_size]
        int real_block = get_real_block(device, ip, logical_block);
        u8* read_buf = get_block(device, real_block);

        /* copy from start_byte to buf[ ], at most remain bytes in this block */
        u8* bp = read_buf + start_byte;   
        int remain = block_size - start_byte;   // number of bytes remain in read_buf[]

        if(avail < remain)
            remain = avail;

        int copy_size = 0;
        if(nbytes < remain)  
            copy_size = nbytes;
        else
            copy_size = remain;

        memcpy(buf, bp, copy_size);

        count      += copy_size;
        fp->offset += copy_size;
        avail      -= copy_size;
        nbytes     -= copy_size;
    }
    printf("read %d bytes from file descriptor %d\n", count, fd);  

    return count;   // count is the actual number of bytes read
}

//(1).  PROC              (2).                          | 
//     =======   |--> OFT oft[ ]                        |
//     | pid |   |   ============                       |
//     | cwd |   |   |mode=flags|                       | 
//     | . ..|   |   |minodePtr ------->  minode[ ]     |      BlockDevice
//     | fd[]|   |   |refCount=1|       =============   |   ==================
// fd: | .------>|   |offset    |       |  INODE    |   |   | INODE -> blocks|
//     |     |       |===|======|       |-----------|   |   ==================
//     =======           |              | dev,inode |   | 
//                       |              |  dirty    |   |
//                       |              =============   |
//                       |
//    -------------------|-----------------
//    |    |    | ...  |lbk  | ............
//    -------------------|-----------------
//lbk   0    1 .....     |rem|            |
//                     start           fileSize (in INODE)  
//                        
//------------------------------------------------------------------------------
//               Data Structures for write()

//mywrite behaves exactly the same as Unix's write(fd, buf, nbytes) syscall.
//It writes nbytes from buf[ ] to the file descriptor fd, extending the file 
//size as needed.
int my_write(int fd, char buf[], int nbytes) 
{
    OPEN_FILE* fp = running->fd[fd]; // Open file table pointer

    // Verify fd is open for write
    if(fp->mode != WR && fp->mode != RW && fp->mode != APPEND)
    {
        fprintf(stderr, "read: failed: file descriptor %d is not open for write\n", fd);
        return FAILURE;
    }

    MINODE* mip = fp->mip;
    INODE* ip = &mip->inode;

    const int device = mip->device;
    const int block_size = get_block_size(device);
    const int file_size = ip->i_size;

    while (nbytes > 0)
    {
        int logical_block = fp->offset / block_size;
        int start_byte    = fp->offset % block_size;

        int real_block = get_real_block(device, ip, logical_block);

        ip->i_block[logical_block] = balloc(device);// MUST ALLOCATE a block

       // IDEA
       // Create a level of abstraction such that
       // On a high level, anything refering to a block is the logical block
       // but on a low level, functions deal with direct/indirect/...
        
        // direct block
        if (logical_block < 12)
        {
            // if no data block yet
            if (ip->i_block[logical_block] == 0)
            {
                ip->i_block[logical_block] = balloc(device);// MUST ALLOCATE a block

                // write a block of 0's to blk on disk: OPTIONAL for data block 
                //                                      but MUST for I or D blocks
            }
            blk = ip->i_block[logical_block];      // blk should be a disk block now
        }
        else if (logical_block >= 12 && logical_block < 256 + 12)
        { 
            // indirect blocks
        }
        else
        {
            // double indirect blocks */
        }

        /* all cases come to here : write to the data block */
        write_buf = get_block(device, blk);   // read disk block into write_buf[ ]  
        char *cp = write_buf + start_byte;      // cp points at start_byte in write_buf[]
        remain = block_size - start_byte;     // number of BYTEs remain in this block

        while (remain > 0){               // write as much as remain allows  
            *cp++ = *cq++;              // cq points at buf[ ]
            nbytes--; remain--;         // dec counts
            oftp->offset++;             // advance offset
            if (offset > ip->i_size)  // especially for RW|APPEND mode
                ip->i_size++;    // inc file size (if offset > fileSize)
            if (nbytes <= 0) break;     // if already nbytes, break
        }
        put_block(device, blk, write_buf);   // write write_buf[ ] to disk

        //// get the data block into read_buf[block_size]
        //int real_block = get_real_block(device, ip, logical_block);
        //u8* read_buf = get_block(device, real_block);

        ///* copy from start_byte to buf[ ], at most remain bytes in this block */
        //u8* bp = read_buf + start_byte;   
        //int remain = block_size - start_byte;   // number of bytes remain in read_buf[]

        //if(avail < remain)
        //    remain = avail;

        //int copy_size = 0;
        //if(nbytes < remain)  
        //    copy_size = nbytes;
        //else
        //    copy_size = remain;

        //memcpy(buf, bp, copy_size);

        //count      += copy_size;
        //fp->offset += copy_size;
        //avail      -= copy_size;
        //nbytes     -= copy_size;


        // loop back to while to write more .... until nbytes are written
    }

    mip->dirty = true;
    printf("wrote %d char into file descriptor fd=%d\n", nbytes, fd);           
    return nbytes;
}
//                OPTIMIZATION OF write Code
//
//As in read(), the above inner while(remain > 0) loop can be optimized:
//Instead of copying one byte at a time and update the control variables on each 
//byte, TRY to copy only ONCE and adjust the control variables accordingly.
//
//REQUIRED: Optimize the write() code in your project.


// Returns NULL if there is not a corresponding OpenFileTable entry
OPEN_FILE* get_file(int fd)
{
    if(fd < 0 || fd >= NFD)
    {
        fprintf(stderr, "get_file: fd = %d:"
                " Invalid file descriptor\n", fd);
        return NULL;
    }

    // Verify running->fd[fd] is pointing at a OFT entry
    for(int i = 0; i < NOFT; i++)
    {
        if(OpenFileTable[i].mip == running->fd[fd]->mip)
            return &OpenFileTable[i];

        if(i == NOFT - 1)
            return NULL;
    }
}

int my_lseek(int fd, int position)
{
    OPEN_FILE* fp = get_file(fd);

    if(!fp)
    {
        fprintf(stderr, "lseek: failed to seek with %d:"
                " File not open\n", fd);
        return FAILURE;
    }

    int start = 0;
    int end   = fp->mip->inode.i_size;

    if(position < start)
        position = start;

    if(position > end) 
        position = end;

    return fp->offset = position;
}

int my_truncate(MINODE *mip)
{
    INODE* ip = &mip->inode;

    const int device = running->cwd->device;
    const int block_size = get_block_size(device);
    const int block_count = ip->i_size / block_size;

    for(int i = 0; i < block_count; i++)
        bfree(device, get_real_block(device, ip, i));

    ip->i_atime = time(0L);
    ip->i_mtime = time(0L);
    ip->i_size = 0;

    mip->dirty = true;
}

int dup(int fd)
{   
    OPEN_FILE* fp = get_file(fd);

    if(!fp)
    {
        fprintf(stderr, "dup: failed to duplicate %d:"
                " File not open\n", fd);
        return FAILURE;
    }

    for(int i; i < NFD; i++)
    {
        if(running->fd[i] == NULL)
        {
            running->fd[i] = fp;
            fp->refCount++;
            return i;
        }

        if(i == NFD - 1)
        {
            fprintf(stderr, "dup: failed to duplicate '%d':"
                    " Process has reached file limit\n", fd);
        }
    }
}

int dup2(int fd, int gd)
{
    close_file(gd);
    dup(fd);
}
