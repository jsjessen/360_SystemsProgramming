#include <cmd.h>

// chown filename 2 
int my_chown(int argc, char* argv[])
{
    const int device = running->cwd->device;

    if(argc < 2)
    {
        fprintf(stderr, "chown: missing operand\n");
        return MISSING_OPERAND;
    }

    char* path = argv[1];

    int ino = getino(device, path);

    if(ino <= 0)
    {
        fprintf(stderr, "chown: failed to change owner of '%s':"
                " No such file or directory\n", path);
        return DOES_NOT_EXIST;
    }

    MINODE* mip = iget(device, ino);
    INODE*   ip = &mip->inode;

    long int new_owner = strtol(argv[2], NULL, 10);
    ip->i_uid = new_owner;

    mip->dirty = true;
    iput(mip);

    return SUCCESS;
}
