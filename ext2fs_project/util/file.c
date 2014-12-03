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
    int device;
    if(pathname[0] == '/')
        device = root->device;
    else
        device = running->cwd->device;

    int ino = getino(device, pathname);
    MINODE* mip = iget(device, ino);
    INODE* ip = &mip->inode; 

    // Verify it exists
    if(!mip)
    {
        fprintf(stderr, "open: failed to open'%s':"
                " Does not exist\n", pathname);
        return FAILURE;
    }
    // Verify it is a regular file
    else if(!S_ISREG(ip->i_mode))
    {
        fprintf(stderr, "open: failed to open'%s':"
                " Not a regular file\n", pathname);
        iput(mip);
        return FAILURE;
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

    // 7 = 111
    u16 mask = mode & (7 << shift);

    int offset;
    switch(mode)
    {
        case RD:
            // 4 = 100
            if(((4 << shift) & mask) == 0 && running->uid != SUPER_USER)
                goto denied;

            offset = 0; 
            break;

        case WR:
            // 2 = 010
            if(((2 << shift) & mask) == 0 && running->uid != SUPER_USER)
                goto denied;

            truncate_file(mip); 
            offset = 0;
            break;

        case RW:
            // 6 = 110 
            if(((6 << shift) & mask) == 0 && running->uid != SUPER_USER)
                goto denied;

            offset = 0; 
            break;

        case APPEND:
            // 2 = 010
            if(((2 << shift) & mask) == 0 && running->uid != SUPER_USER)
                goto denied;

            offset = ip->i_size;  
            break;

        default:
            fprintf(stderr, "open: failed to open '%s':"
                    " Unknown mode\n", pathname);
            iput(mip);
            return FAILURE;

denied:
            fprintf(stderr, "open: Permission denied\n");
            iput(mip);
            return FAILURE;
    }

    // Find process's first open fd 
    int fd;
    for(fd = 0; fd < NFD; fd++)
    {
        if(running->fd[fd] == NULL)
            break;

        if(fd == NFD - 1)
        {
            fprintf(stderr, "open: failed to open '%s':"
                    " Process has reached file limit\n", pathname);
            iput(mip);
            return FAILURE;
        }
    }

    OPEN_FILE* fp = NULL;
    for(int i = 0; i < NOFT; i++)
    {
        fp = &OpenFileTable[i];

        // Check if the file is already open
        // and verify it is open with a compatible mode
        if(mode != RD 
                && fp->refCount > 0
                && fp->mip == mip
                && fp->mode != RD)
        {
            fprintf(stderr, "open: failed to open '%s':"
                    " File in use\n", pathname);
            iput(mip);
            return FAILURE;
        }

        // Make entry in first not in use OpenFileTable entry
        if(fp->refCount == 0)
        {
            fp->mode = mode;
            fp->offset = offset;
            fp->refCount++;
            fp->mip = mip;

            // Add fd to process
            running->fd[fd] = fp;

            break;
        }

        // No more available space in the OpenFileTable
        if(i == NOFT - 1)
        {
            fprintf(stderr, "open: failed to open '%s':"
                    " Too many files open\n", pathname);
            iput(mip);
            return FAILURE;
        }
    }

    // Update inode's time field
    ip->i_atime = time(0L);
    mip->dirty = true;

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

    OPEN_FILE* fp = NULL;

    // Verify running->fd[fd] is pointing at a OFT entry
    for(int i = 0; i < NOFT; i++)
    {
        fp = &OpenFileTable[i];

        if(fp->mip == running->fd[fd]->mip)
            break;

        if(i == NOFT - 1)
        {
            fprintf(stderr, "close: failed to close %d:"
                    " File not in OpenFileTable\n", fd);
            return FAILURE;
        }
    }

    fp = running->fd[fd];
    running->fd[fd] = NULL;
    fp->refCount--;

    // last user of this OFT entry ==> dispose of the Minode[]
    if(fp->refCount == 0) 
        iput(fp->mip);

    return SUCCESS; 
}

// (1).  PROC              (2).                          | 
//      =======   |--> OFT oft[ ]                        |
//      | pid |   |   ============                       |
//      | cwd |   |   |mode=flags|                       | 
//      | . ..|   |   |minodePtr ------->  minode[ ]     |      BlockDevice
//      | fd[]|   |   |refCount=1|       =============   |   ==================
//  fd: | .------>|   |offset    |       |  INODE    |   |   | INODE -> blocks|
//      |     |       |===|======|       |-----------|   |   ==================
//      =======           |              | dev,inode |   | 
//                        |              |  dirty    |   |
//                        |              =============   |
//                        |
//     -------------------|-----------------
//     |    |    | ...  |lbk  | ............
//     -------------------|-----------------
// lbk   0    1 .....     |rem|            |
//                      start           fileSize (in INODE)  

// It tries to read nbytes from fd to buf[ ], and returns the 
// actual number of bytes read.
int read_file(int fd, char* buf, int nbytes)
{
    OPEN_FILE* fp = running->fd[fd]; // Open file table pointer

    // Verify fd is open for read
    if(!fp || (fp->mode != RD && fp->mode != RW))
    {
        fprintf(stderr, "read: failed to read %d:"
                " not open for read\n", fd);
        return FAILURE;
    }

    MINODE* mip = fp->mip;
    INODE* ip = &mip->inode;

    const int device = mip->device;
    const int block_size = get_block_size(device);
    const int file_size = ip->i_size;

    int count = 0;
    int available = file_size - fp->offset; // number of bytes still available in file

    while(nbytes && available)
    {
        //Compute LOGICAL BLOCK number logical_block and start_byte in that block from offset;
        int logical_block = fp->offset / block_size;
        int start_byte    = fp->offset % block_size;

        // get the data block into read_buf[block_size]
        int real_block = get_logical_bno(device, ip, logical_block);

        //printf("READ REAL = %d\n", real_block);
        //printf("i_block[0] = %d\n", ip->i_block[0]);
        u8* read_buf = get_block(device, real_block);

        // copy from start_byte to buf[ ], at most remain bytes in this block
        u8* bp = read_buf + start_byte;   
        int remain = block_size - start_byte;   // number of bytes remain in read_buf[]

        if(available < remain)
            remain = available;

        int copy_size;
        if(nbytes < remain)  
            copy_size = nbytes;
        else
            copy_size = remain;

        memcpy(buf, bp, copy_size);

        count      += copy_size;
        fp->offset += copy_size;
        available  -= copy_size;
        nbytes     -= copy_size;
    }

    return count;   // count is the actual number of bytes read
}

//mywrite behaves exactly the same as Unix's write(fd, buf, nbytes) syscall.
//It writes nbytes from buf[ ] to the file descriptor fd, extending the file 
//size as needed.
int write_file(int fd, char buf[], int nbytes) 
{
    OPEN_FILE* fp = running->fd[fd]; // Open file table pointer

    // Verify fd is open for read
    if(!fp || (fp->mode != WR && fp->mode != RW && fp->mode != APPEND))
    {
        fprintf(stderr, "write: failed to write to %d:"
                " not open for write\n", fd);
        return FAILURE;
    }

    MINODE* mip = fp->mip;
    INODE* ip = &mip->inode;

    const int device = mip->device;
    const int block_size = get_block_size(device);

    int count = 0;

    while(nbytes)
    {
        //Compute LOGICAL BLOCK number logical_block and start_byte in that block from offset;
        int logical_block = fp->offset / block_size;
        int start_byte    = fp->offset % block_size;

        // get the data block into read_buf[block_size]
        int real_block = get_logical_bno(device, ip, logical_block);
        if(real_block == 0)
            real_block = logical_balloc(device, ip);

        printf("WRITE REAL = %d\n", real_block);
        for(int i = 0; i < 15; i++)
            printf("i_block[%d] = %d\n", i, ip->i_block[i]);
        print_divider('-');

        u8* write_buf = get_block(device, real_block);

        u8* bp = write_buf + start_byte;      // cp points at start_byte in write_buf[]
        int remain = block_size - start_byte;     // number of BYTEs remain in this block

        int copy_size;
        if(nbytes < remain)  
            copy_size = nbytes;
        else
            copy_size = remain;

        memcpy(bp, buf, copy_size);
        put_block(device, real_block, write_buf);   // write write_buf[ ] to disk
        //puts(buf);

        count      += copy_size;
        fp->offset += copy_size;
        nbytes     -= copy_size;

        // increase file size if offset is greater
        if (fp->offset > ip->i_size)
            ip->i_size = fp->offset;
    }
    //printf("\nWrote %d bytes to file descriptor %d\n", count, fd);  

    if(count > 0)
        mip->dirty = true;

    return count;
}

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
            return running->fd[fd];
    }

    return NULL;
}

int seek_file(int fd, int position)
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

int truncate_file(MINODE *mip)
{
    INODE* ip = &mip->inode;

    const int device = running->cwd->device;
    const int block_size = get_block_size(device);
    const int block_count = ip->i_size / block_size;

    for(int i = 0; i < block_count; i++)
        bfree(device, get_logical_bno(device, ip, i));

    ip->i_atime = time(0L);
    ip->i_mtime = time(0L);
    ip->i_size = 0;

    mip->dirty = true;

    return SUCCESS;
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

    return FAILURE;
}

int dup2(int fd, int gd)
{
    if(close_file(gd) == FAILURE)
        return FAILURE;

    if(dup(fd) == FAILURE)
        return FAILURE;

    return SUCCESS;
}
