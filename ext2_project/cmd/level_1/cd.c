#include <cmd.h>

// cd => cd / => cwd = /
// cd dir   =>   cwd = dir
int my_cd(int argc, char* argv[])
{
    int ino = 0;
    int dev = running->cwd->dev;

    MINODE* mip = NULL;

    // If no arg, cd to root
    if(argc < 2)
    {
        running->cwd = root;
        running->cwd->refCount++;
        return 1;
    }

    // Get dir inode in memory
    ino = getino(dev, argv[1]);
    mip = iget(dev, ino);

    // Check that arg is a dir
    if(!S_ISDIR(mip->inode.i_mode))
    {
        iput(mip);
        fprintf(stderr, "cd: %s: Not a directory\n", argv[1]);
        return 0;
    }

    iput(running->cwd);
    running->cwd = mip;
    running->cwd->refCount++;

    return 1;
}
