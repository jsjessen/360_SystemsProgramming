#include <cmd.h>

//int read_file()
//{
//  Preparations: 
//    ASSUME: file is opened for RD or RW;
//    ask for a fd  and  nbytes to read;
//    verify that fd is indeed opened for RD or RW;
//    return(myread(fd, buf, nbytes));
//}

// The algorithm of myread() can be best explained in terms of the following 
// diagram.
// 
// (1).  PROC              (2).                          | 
//      =======   |--> OFT oft[ ]                        |
//      | pid |   |   ============                       |
//      | cwd |   |   |mode=flags|                       | 
//      | . ..|   |   |minodePtr ------->  minode[ ]     |      BlockDevice
//      | fd[]|   |   |refCount=1|       =============   |   ==================
//  fd: | .------>|   |offset    |       |  INODE    |   |   | INODE -> blocks|
//      |     |       |===|======|       |-----------|   |   ==================
//      =======           |              |  dev,ino  |   |
//                        |              =============   |
//                        |
//                        |<------- avail ------->|
//     -------------------|-----------------------
//     |    |    | ...  |logic_block  |   |  ...| .......|
//     -------------------|---|------------------|-
// logic_block   0    1 .....     |rem|                   |
//                      start                   fsize  
//                         
// ------------------------------------------------------------------------------
//                  Data structures for reading file
// 
// (1). Assume that fd is opened for READ. 
// (2). The offset in the OFT points to the current byte position in the file from
//      where we wish to read nbytes. 
// (3). To the kernel, a file is just a sequence of contiguous bytes, numbered from
//      0 to file_size - 1. As the figure shows, the current byte position, offset
//      falls in a LOGICAL block (logic_block), which is 
//                logic_block = offset / block_size 
// 
//      the byte to start read in that logical block is 
//              start = offset % block_size 
// 
//      and the number of bytes remaining in the logical block is 
//             remain = block_size - start. 
// 
//      At this moment, the file has 
//                avail = file_size-offset 
// 
//      bytes available for read. 

// It tries to read nbytes from fd to buf[ ], and returns the 
// actual number of bytes read.
int my_read(int fd, char* buf, int nbytes)
{
    OPEN_FILE* fp = running->fd[fd]; // Open file table pointer

    if(fp->mode != R && fp->mode != RW)
    {
        fprintf(stderr, "read: failed: file descriptor %d is not open for read\n", fd);
        return 0;
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
        //Compute LOGICAL BLOCK number logic_block and startByte in that block from offset;
        int logic_block = fp->offset / block_size;
        int startByte   = fp->offset % block_size;

        // get the data block into read_buf[block_size]
        u8* read_buf = get_real_block(device, logic_block);

        /* copy from startByte to buf[ ], at most remain bytes in this block */
        u8* bp = read_buf + startByte;   
        int remain = block_size - startByte;   // number of bytes remain in read_buf[]

        if(avail < remain)
            remain = avail;

        int num_bytes_copy = 0;
        if(nbytes < remain)  
            num_bytes_copy = nbytes 
        else
            num_bytes_copy = remain;

        int bytes_copied = memcpy(buf, bp, num_bytes_copy);

        if(bytes_copied < num_bytes_copy)
            fprintf(stderr, "read: partial memcpy\n");

        count      += bytes_copied;
        fp->offset += bytes_copied;
        avail      -= bytes_copied;
        nbytes     -= bytes_copied;
    }
    printf("read %d bytes from file descriptor %d\n", count, fd);  

    return count;   // count is the actual number of bytes read
}
