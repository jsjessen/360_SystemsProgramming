#include <cmd.h>

// Update access and modification times for file
int my_touch(int argc, char* argv[])
{
    result_t result = NONE;
    const int device = running->cwd->device;

    if(argc < 2)
    {
        fprintf(stderr, "touch: missing operand\n");
        return MISSING_OPERAND;
    }

    // touch each filename given by user
    int i = 1;
    while(i < argc)
    {
        char* path = argv[i];

        // Target exists
        int target_ino = getino(device, path);
        if(target_ino > 0)
        {
            MINODE* target_mip = iget(device, target_ino);
            INODE* target_ip = &target_mip->inode;

            target_ip->i_atime = time(0L);
            target_mip->dirty = true;

            iput(target_mip);
        }
        else // Target does not exist
        {
            char* new_argv[] = { "creat", argv[i], NULL };
            result = my_creat(2, new_argv);
        }

        if(result != NONE)
            return result;

        i++;
    }

    return SUCCESS;
}
