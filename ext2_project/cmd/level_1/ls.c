#include <cmd.h>

// ls     => ls <cwd>
// ls dir => ls <cwd>/dir
// ls a/b/c 
// ls a/b/c e/f/g /h/i/j
int my_ls(int argc, char* argv[])
{
    int ino = 0;
    int dev = running->cwd->dev;
    MINODE *mip = NULL;

    // If given no path, ls cwd
    if(argc < 2)
    {
        list_dir(running->cwd);
        return 0;
    }

    // ls each path given by user
    int i = 0;
    while(argv[++i])
    {
        // If printing multiple lists
        // label each one
        if(argc > 2)
            printf("%s:\n", argv[i]);

        if((ino = getino(dev, argv[i])) < 0)
        {
            printf("ls: cannot access %s: no such file or directory\n", argv[i]);
            return -1;
        }
        mip = iget(dev, ino);

        if(S_ISDIR(mip->inode.i_mode))
            list_dir(mip);
        else
            list_file(mip, argv[i]);

        iput(mip);
    }

    return 0;
}
