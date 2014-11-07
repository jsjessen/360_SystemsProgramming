int quit(int argc, char* argv[])
{
    int i = 0;

    for(i = 0; i < NMINODES; i++)
    {
        MINODE* mip = &MemoryInodeTable[i];

        mip->refCount = 1;
        iput(mip);
    }
    exit(0);
}
