#include <cmd.h>

char* readlink(char* pathname);


// symlink targetFileName linkFileName
int my_symlink(int argc, char* argv[])
{
    const int device = running->cwd->device;

    if(argc < 3)
    {
        fprintf(stderr, "symlink: missing operand\n");
        return FAILURE;
    }

    char* target_name = argv[1];
    char* link_name = argv[2];

    // Get the inode into memory
    int target_ino = getino(device, target_name);
    MINODE* target_mip = iget(device, target_ino);

    // Verify that inode exists
    // Linux actually allows you to create broken links
    if(!target_mip)
    {
        fprintf(stderr, "symlink: failed to access '%s':"
                " No such file or directory\n", target_name);
        goto clean_up;
    }
    // i_block is 4 * 15 = 60 bytes
    // There are 20 bytes of reserved space after i_block (KC counts 24)
    // Lets be safe and just cutoff at 60 (don't forget null char)
    else if(strlen(target_name) >= 60)
    {
        fprintf(stderr, "symlink: failed to create symbolic link '%s' => '%s':"
                " Name of target too long\n", link_name, target_name);
        goto clean_up;
    }

    // From link_name, get path to parent and name of child
    char* link_parent_name = NULL;
    char* link_child_name  = NULL;
    parse_path(link_name, &link_parent_name, &link_child_name);

    // Get parent in memory
    int link_parent_ino = getino(device, link_parent_name);
    MINODE* link_parent_mip = iget(device, link_parent_ino);

    // Verify that linkParent exists
    if(!link_parent_mip)
    {
        fprintf(stderr, "symlink: failed to create symbolic link '%s' => '%s':"
                " No such file or directory\n", link_name, target_name);
        goto clean_up_more;
    }
    // Verify that linkParent is a directory
    else if(!S_ISDIR(link_parent_mip->inode.i_mode))
    {
        fprintf(stderr, "symlink: failed to access '%s':"
                " Not a directory\n", link_name);
        goto clean_up_more;
    }
    // Verify that linkChild does not yet exist
    else if(getino(device, link_name) > 0)
    {
        fprintf(stderr, "symlink: failed to create symbolic link '%s':"
                " File exists\n", link_name);
        goto clean_up_more;
    }

    // Make a file with the child's name in the parent directory
    int link_ino = creat_file(link_parent_mip, link_child_name);
    MINODE* link_mip = iget(device, link_ino);
    INODE* link_ip = &link_mip->inode; 

    // Change the file's type to link
    // The leading 4 bits of type indicate type
    // First clear its type by ANDing with 0x0FFF
    // Then set its type to link by ORing with 0xA000
    //link_ip->i_mode = (link_ip->i_mode & 0x0FFF) | 0xA000;
    link_ip->i_mode = LINK_MODE;
    
    // write the string target_name into the link's i_block[]
    strcpy((char*)(link_ip->i_block), target_name);
    link_ip->i_size = strlen(target_name);

    link_mip->dirty = true;
    iput(link_mip);

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
    iput(target_mip); 

    char* contents = readlink(link_name);
    printf("READ LINK: '%s'\n", contents);
    free(contents);

    return SUCCESS;
}

// returns the contents of a symlink file 
char* readlink(char* pathname)
{
    const int device = running->cwd->device;

    // Get the inode into memory
    int ino = getino(device, pathname);
    MINODE* mip = iget(device, ino);
    INODE* ip = &mip->inode;

    char* result = NULL;

    if(!S_ISLNK(ip->i_mode))
    {
        fprintf(stderr, "readlink: '%s' is not a symlink", pathname);
        goto clean_up;
    }
    
    char* contents = (char*)(ip->i_block);
    result = (char*)malloc((strlen(contents) + 1) * sizeof(char));
    strcpy(result, contents);

clean_up:
    iput(mip);

    return result;
}
