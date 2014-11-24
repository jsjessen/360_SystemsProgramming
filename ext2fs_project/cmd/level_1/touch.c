#include <cmd.h>

// Update access and modification times for file
int my_touch(int argc, char* argv[])
{
    const int device = running->cwd->device;

    if(argc < 2)
    {
        fprintf(stderr, "touch: missing operand\n");
        return FAILURE;
    }

    // touch each filename given by user
    int i = 1;
    while(i < argc)
    {
        char* path = argv[i];

        // From path, get path to parent and name of child
        char* parent_name = NULL;
        char* child_name  = NULL;
        parse_path(path, &parent_name, &child_name);

        // Get parent in memory
        int parent_ino = getino(device, parent_name);
        MINODE* parent_mip = iget(device, parent_ino);

        // Verify that parent exists
        if(!parent_mip)
        {
            fprintf(stderr, "touch: cannot touche file '%s':"
                    " No such file or directory\n", path);
            goto clean_up;
        }
        // Verify that parent is a directory
        else if(!S_ISDIR(parent_mip->inode.i_mode))
        {
            fprintf(stderr, "touch: cannot touche file '%s':"
                    " Not a directory\n", path);
            goto clean_up;
        }
        // Verify that child does not yet exist
        else if(getino(device, path) > 0)
        {
            fprintf(stderr, "touch: cannot touche file '%s':"
                    " File exists\n", path);
            goto clean_up;
        }

        // If touching multiple files
        if(argc > 2)
            printf("touch: touching directory '%s'\n", path);

        INODE*  parent_ip  = &parent_mip->inode;

        // Make a file with the child's name in the parent directory
        creat_file(parent_mip, child_name);

        // Set parent's last time of access to current time
        parent_ip->i_atime = time(0L);

        // Parent memory-inode now has child in its directory
        // but the disk-inode does not, hence parent is dirty.
        parent_mip->dirty = true;

clean_up:
        // Move parent inode from memory to disk
        iput(parent_mip); 

        free(parent_name);
        free(child_name);

        i++;
    }

    return SUCCESS;
}
