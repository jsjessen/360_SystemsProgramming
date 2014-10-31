// James Jessen
// 10918967   
// 
// CptS 360

#include "util.h"
#include "type.h"

// return minode pointer to loaded INODE
INODE *iget(int dev, int ino)
{
    // Once you have the ino of an inode, you may load the inode into a slot
    // in the Minode[] array. 
    int i;

    // To ensure uniqueness, you must search the Minode[] 
    // array to see whether the needed INODE already exists
    for(i = 0; i < NMINODES; i++)
    {
        mip = &minode[i];
        if(mip->dev == dev && mip->ino == ino)
        {
            // If you find the needed INODE already in a Minode[] slot, just inc its 
            // refCount by 1 and return the Minode[] pointer.
            mip->refCount++;
            return mip;
        }
    }

    // If you do not find it in memory
    for(i = 0; i < NMINODES; i++)
    {
        mip = &minode[i];
        if(mip->refCount == 0)
        {
            // you must allocate a FREE Minode[i], load
            // the INODE from disk into that Minode[i].INODE, initialize the Minode[]'s
            // other fields and return its address as a MINODE pointer,

            mip->refCount = 1;
            mip->dev = dev;
            mip->ino = ino;

            //get INODE of ino to buf
            blk = (ino-1)/8 + inode_start;
            disp = (ino-1)%8;

            get_block(dev, blk, buf);
            ip = (INODE*)buf + disp;

            // copy INODE to mp->INODE
            mip->INODE = *ip;
            // same as memcpy(mip->INODE, ip, sizeof(INODE));

            return mip;
        }
    }

     

        int iput(MINODE *mip)
        {
              // This function releases a Minode[]. Since an Minode[]'s refCount indicates
                  // the number of users on this Minode[], releasing is done as follows:
                  //  
                  // First, dec the refCount by 1. 
                  // If (after dec) refCount > 0 ==> return;
              // if Minode[].dirty == 0 ==> no need to write back, so return;
              // --------------------------------------------------------------
                  // Otherwise, (dirty==1) ==> must write the INODE back to disk.
                  //  
                  // To write an INODE back to disk:
                  //    Use Minode's (dev, ino) to determine which dev and which INODE on disk,
                  // i.e. use Mailman's algorithm to determine disk block and which inode in that 
                      // block.
                      // Read that block in, copy Minode's INODE into the INODE area in that block
                      // and write the block back to disk.
        } 



    int test_bit(u8* buf, int bit)
    {
        int byte = bit / 8; 
        bit %= 8;

        if (buf[byte] & (1 << bit))
            return 1;
        else
            return 0;
    }

    void set_bit(u8** buf, int bit)
    {
        int byte = bit / 8; 
        bit %= 8;

        (*buf)[byte] |= (1 << bit);
    }

    void clear_bit(u8** buf, int bit)
    {
        int byte = bit / 8; 
        bit %= 8;

        (*buf)[byte] &= ~(1 << bit);
    }
