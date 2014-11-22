#include <cmd.h>

// link oldFileName newFileName
//
// creates a file newFileName which has 
// the same inode(number) as that of oldfileName
int my_link(int argc, char* argv[])
{
    const int device = running->cwd->device;

    if(argc < 3)
    {
        fprintf(stderr, "link: missing operand\n");
        return FAILURE;
    }

    char* old_path = argv[1];
    char* new_path = argv[2];

    // Get the inode into memory
    int ino = getino(device, old_path);
    MINODE* mip = iget(device, ino);

    // Verify that inode exists
    if(!mip)
    {
        fprintf(stderr, "link: failed to access '%s':"
                " No such file or directory\n", old_path);
        goto clean_up;
    }
    // Verify that oldFile is not a directory
    else if(S_ISDIR(mip->inode.i_mode))
    {
        fprintf(stderr, "link: '%s':"
                " hard link not allowed for directory\n", old_path);
        goto clean_up;
    }

    // From new_path, get path to parent and name of child
    char* new_parent_name = NULL;
    char* new_child_name  = NULL;
    parse_path(new_path, &new_parent_name, &new_child_name);

    // Get parent in memory
    int new_parent_ino = getino(device, new_parent_name);
    MINODE* new_parent_mip = iget(device, new_parent_ino);

    // Verify that newParent exists
    if(!new_parent_mip)
    {
        fprintf(stderr, "link: failed to create hard link '%s' => '%s':"
                " No such file or directory\n", new_path, old_path);
        goto clean_up_more;
    }
    // Verify that newParent is a directory
    else if(!S_ISDIR(new_parent_mip->inode.i_mode))
    {
        fprintf(stderr, "link: failed to access '%s':"
                " Not a directory\n", new_path);
        goto clean_up_more;
    }
    // Verify that newChild does not yet exist
    else if(getino(device, new_path) > 0)
    {
        fprintf(stderr, "link: failed to create hard link '%s':"
                " File exists\n", new_path);
        goto clean_up_more;
    }

    // Verify that link is not being made across devices
    if(mip->device != new_parent_mip->device)
    {
        fprintf(stderr, "link: failed to create hard link '%s' => '%s':"
                " Invalid cross-device link\n", new_path, old_path);
        goto clean_up_more;
    }

    // Make entry for newFile in newParent directory
    // with the same inode number as oldFile
    enter_name(new_parent_mip, ino, new_child_name);

    INODE* new_parent_ip  = &new_parent_mip->inode;

    new_parent_ip->i_links_count++;

    // Make a file with the child's name in the parent directory
    //creat_file(parent_mip, child_name);

    // Set parent's last time of access to current time
    new_parent_ip->i_atime = time(0L);

    // Parent memory-inode now has child in its directory
    // but the disk-inode does not, hence parent is dirty.
    new_parent_mip->dirty = true;

clean_up_more:
    iput(new_parent_mip); 

    free(new_parent_name);
    free(new_child_name);

clean_up:
    // Move parent inode from memory to disk
    iput(mip); 

    return SUCCESS;
}
