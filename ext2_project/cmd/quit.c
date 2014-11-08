#include <cmd.h>

int quit(int argc, char* argv[])
{
    int i = 0;
    MINODE* mip = NULL;
    
    for(i = 0; i < NMINODES; i++)
    {
        mip = &MemoryInodeTable[i];

        if(mip->refCount > 0)
            iput(mip);
    }
    exit(0);
}
