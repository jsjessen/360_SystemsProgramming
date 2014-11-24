#include <cmd.h>

// chown filename 2 
int my_chown(int argc, char* argv[])
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

    long int new_owner = strtol(argv[2], NULL, 10);
    ip->i_uid = new_owner;

    mip->dirty = true;
    iput(mip);

    return SUCCESS;
}
