#include <cmd.h>

// Need to limit which users can change permissions

// chmod filename 644 
int my_chmod(int argc, char* argv[])
{
    const int device = running->cwd->device;

    if(argc < 2)
    {
        fprintf(stderr, "chmod: missing operand\n");
        return FAILURE;
    }

    char* path = argv[1];

    int ino = getino(device, path);
    MINODE* mip = iget(device, ino);
    INODE*   ip = &mip->inode;

    long int new_mode = strtol(argv[2], NULL, 8);

    // Change the file's mode to new_mode 
    // The leading 4 bits of type indicate type
    // First clear its current mode by ANDing with 0xF000
    // Then set its mode by ORing with new_mode 
    ip->i_mode = (ip->i_mode & 0xF000) | new_mode;

    mip->dirty = true;
    iput(mip);

    return SUCCESS;
}
