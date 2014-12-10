#include <cmd.h>

// link targetFileName linkFileName
//
// creates a file linkFileName which has 
// the same inode(number) as that of targetfileName
int my_link(int argc, char* argv[])
{
    result_t result = NONE;
    const int device = running->cwd->device;

    if(argc < 3)
    {
        fprintf(stderr, "link: missing operand\n");
        return MISSING_OPERAND;
    }

    char* target_pathname = argv[1];
    char* link_pathname = argv[2];

    // Get the inode into memory
    int ino = getino(device, target_pathname);
    MINODE* mip = iget(device, ino);

    // Verify that inode exists
    if(!mip)
    {
        result = DOES_NOT_EXIST;
        fprintf(stderr, "link: failed to access '%s':"
                " No such file or directory\n", target_pathname);
        goto clean_up;
    }
    // Verify that targetFile is not a directory
    else if(S_ISDIR(mip->inode.i_mode))
    {
        result = IS_DIR;
        fprintf(stderr, "link: '%s':"
                " hard link not allowed for directory\n", target_pathname);
        goto clean_up;
    }

    // From link_pathname, get path to parent and name of child
    char* link_parent_name = NULL;
    char* link_child_name  = NULL;
    parse_path(link_pathname, &link_parent_name, &link_child_name);

    // Get parent in memory
    int link_parent_ino = getino(device, link_parent_name);
    MINODE* link_parent_mip = iget(device, link_parent_ino);

    // Verify that linkParent exists
    if(!link_parent_mip)
    {
        result = NO_PARENT;
        fprintf(stderr, "link: failed to create hard link '%s' => '%s':"
                " No such file or directory\n", link_pathname, target_pathname);
        goto clean_up_more;
    }
    // Verify that linkParent is a directory
    else if(!S_ISDIR(link_parent_mip->inode.i_mode))
    {
        result = PARENT_NOT_DIR;
        fprintf(stderr, "link: failed to access '%s':"
                " Not a directory\n", link_pathname);
        goto clean_up_more;
    }
    // Verify that linkChild does not yet exist
    else if(getino(device, link_pathname) > 0)
    {
        result = ALREADY_EXISTS;
        fprintf(stderr, "link: failed to create hard link '%s':"
                " File exists\n", link_pathname);
        goto clean_up_more;
    }

    // Verify that link is not being made across devices
    if(mip->device != link_parent_mip->device)
    {
        result = CROSS_DEVICE;
        fprintf(stderr, "link: failed to create hard link '%s' => '%s':"
                " Invalid cross-device link\n", link_pathname, target_pathname);
        goto clean_up_more;
    }

    // Make entry for linkFile in linkParent directory
    // with the same inode number as targetFile
    enter_name(link_parent_mip, ino, link_child_name);

    INODE* ip = &mip->inode;

    // Now there is another file linked to the inode
    // so increment number of links
    ip->i_links_count++;
    mip->dirty = true;

    INODE* link_parent_ip  = &link_parent_mip->inode;

    // Set parent's last time of access to current time
    link_parent_ip->i_atime = time(0L);
    link_parent_mip->dirty = true;

clean_up_more:
    iput(link_parent_mip); 

    free(link_parent_name);
    free(link_child_name);

clean_up:
    // Move parent inode from memory to disk
    iput(mip); 

    if(result != NONE)
        return result;

    return SUCCESS;
}
