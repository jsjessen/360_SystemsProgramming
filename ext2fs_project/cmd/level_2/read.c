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
//     |    |    | ...  |lbk  |   |  ...| .......|
//     -------------------|---|------------------|-
// lbk   0    1 .....     |rem|                   |
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
//      falls in a LOGICAL block (lbk), which is 
//                lbk = offset / block_size 
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
// 
//      These numbers are used in the read algorithm.

// It tries to read nbytes from fd to buf[ ], and returns the 
// actual number of bytes read.
int my_read(int fd, char* buf, int nbytes)
{
    OPEN_FILE* fp = running->fd[fd]; // Open file table pointer
    MINODE* mip = fp->mip;
    INODE* ip = &mip->inode;

    const int device = mip->device;
    const int block_size = get_block_size(device);
    const int file_size = ip->i_size;
    
    const int block_num_per_single = block_num_per_single;
    const int block_num_per_double = block_num_per_double;
    const int block_num_per_triple = block_num_per_double * block_num_per_single;

    int count = 0;
    int avail = file_size - fp->offset; // number of bytes still available in file.
    char *cq = buf;   // cq points at buf[ ]

    while (nbytes && avail)
    {
        //Compute LOGICAL BLOCK number lbk and startByte in that block from offset;
        int lbk       = fp->offset / block_size;
        int startByte = fp->offset % block_size;


        if (lbk < NUM_DIRECT_BLOCKS)                     
        {
            // direct blocks
            blk = ip->i_block[lbk]; // map lbk to blk
        }
        else if (lbk >= NUM_DIRECT_BLOCKS && lbk < NUM_DIRECT_BLOCKS + block_num_per_single) 
        {
            int passed = NUM_DIRECT_BLOCKS;

            // indirect blocks 
            int* single_buf[block_num_per_single];
            get_block(device, ip->i_block[NUM_DIRECT_BLOCKS], single_buf);

            blk = single_buf[lbk - passed];
        }
        else if (lbk >= NUM_DIRECT_BLOCKS && lbk < NUM_DIRECT_BLOCKS + block_num_per_double)
        { 
            int passed = NUM_DIRECT_BLOCKS + block_num_per_single;

            // double indirect blocks
            int* double_buf[block_num_per_single];
            get_block(device, ip->i_block[NUM_DIRECT_BLOCKS + 1], double_buf);

            int double_block = (lbk - passed) / block_num_per_single;
            int single_block = (lbk - passed) % block_num_per_single;

            int* single_buf[block_num_per_single];
            get_block(device, double_buf[double_block], single_buf);

            blk = single_buf[single_block];
        } 
        else
        {
            //  triple indirect blocks
            int passed = NUM_DIRECT_BLOCKS + block_num_per_single + block_num_per_double;;

            // double indirect blocks
            int* double_buf[block_num_per_single];
            get_block(device, ip->i_block[NUM_DIRECT_BLOCKS + 1], double_buf);

            int double_block = (lbk - passed) / block_num_per_single;
            int single_block = (lbk - passed) % block_num_per_single;

            int* single_buf[block_num_per_single];
            get_block(device, double_buf[double_block], single_buf);

            blk = single_buf[single_block];
        }

        // get the data block into readbuf[block_size]
        get_block(device, blk, readbuf);

        // Instead of reading one byte at a time and updating the counters on each byte,
        // try to calculate the maximum number of bytes available in a data block and
        // the number of bytes still needed to read. Take the minimum of the two, and read
        // that many bytes in one operation. Then adjust the counters accordingly. This 
        // would make the read loops more efficient. 

        /* copy from startByte to buf[ ], at most remain bytes in this block */
        char *cp = readbuf + startByte;   
        remain = block_size - startByte;   // number of bytes remain in readbuf[]

        while (remain > 0)
        {
            *cq++ = *cp++;   // copy byte from readbuf[] into buf[]
            fp->offset++;    // advance offset 
            count++;         // inc count as number of bytes read
            avail--; 
            nbytes--;  
            remain--;

            if (nbytes <= 0 || avail <= 0) 
                break;
        }
        // if one data block is not enough, loop back to OUTER while for more ...
    }
    printf("myread: read %d char from file descriptor %d\n", count, fd);  
    return count;   // count is the actual number of bytes read
}
