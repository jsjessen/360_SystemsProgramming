#include <cmd.h>
 
// Change the current working directory to
// the first argument after the 'cd' command
// If none exists, root becomes the new cwd
int my_cd(int argc, char* argv[])
{
    const int device = running->cwd->device;
    MINODE* cwd = running->cwd;

    // If no arg, cd to root
    if(argc < 2)
    {
        iput(cwd);
        running->cwd = root;
        running->cwd->refCount++;
        return SUCCESS;
    }

    // Get dir inode in memory
    int ino = getino(device, argv[1]);
    MINODE* new_cwd = iget(device, ino);

    // Check that arg is a dir
    if(!S_ISDIR(new_cwd->inode.i_mode))
    {
        iput(new_cwd);
        fprintf(stderr, "cd: %s: Not a directory\n", argv[1]);
        return FAILURE;
    }

    iput(cwd);
    running->cwd = new_cwd;

    return SUCCESS;
}
