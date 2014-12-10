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

    int ino = getino(device, argv[1]);

    // Verify that the file exists
    if(ino < ROOT_INODE)
    {
        fprintf(stderr, "cd: %s: No such file or directory\n", argv[1]);
        return DOES_NOT_EXIST;
    }

    // Get dir inode in memory
    MINODE* new_cwd = iget(device, ino);

    // Check that the file is a directory 
    if(!S_ISDIR(new_cwd->inode.i_mode))
    {
        iput(new_cwd);
        fprintf(stderr, "cd: %s: Not a directory\n", argv[1]);
        return NOT_DIR;
    }

    iput(cwd);
    running->cwd = new_cwd;

    return SUCCESS;
}
