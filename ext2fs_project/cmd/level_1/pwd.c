#include <cmd.h>

int rpwd(MINODE* mip, char** path, int size);


// print the pathname of current working directory 
int my_pwd(int argc, char* argv[])
{
    int initial_size = 128; // Minimum: root(/) + null

    // Allocate memory for the path of the cwd
    char* path = NULL;
    if((path = (char*)malloc(initial_size * sizeof(char*))) == NULL)
    {
        perror("pwd: path initial malloc");
        return FAILURE;
    } 

    // set first char to null for strcat (null = where to start cat)
    path[0] = '\0';

    // rpwd assumes mip was obtained with iget, iputs accordingly
    running->cwd->refCount++;

    if(rpwd(running->cwd, &path, initial_size))
        puts(path);

    free(path);
    return SUCCESS;
}


int rpwd(MINODE* mip, char** path, int size)
{
    const int device = running->cwd->device;

    // Base case: check if at root
    if(mip == root)
    {
        iput(mip);
        strcat(*path, "/");
        return SUCCESS;
    }

    // Check . and .. in my directory for my/parent's inode numbers 
    int my_ino = 0;
    int parent_ino = 0;
    findino(mip, &my_ino, &parent_ino);

    // Get my parent directory's MINODE
    MINODE* parent_mip = iget(device, parent_ino);

    // Do the same for parent 
    if(!rpwd(parent_mip, path, size))
    {
        iput(mip);
        return FAILURE;
    }
    // path = /a/b/.../<need to add my name here>

    // Look in parent directory for my inode number to get my name
    char* my_name = NULL;
    findmyname(parent_mip, my_ino, &my_name);

    // path + my_name + / + null + root(/)
    while(strlen(*path) + strlen(my_name) + 3 > size)
    {
        // Double size of path
        size += size;

        if((*path = (char*)realloc(*path, size * sizeof(char))) == NULL)
        {
            iput(mip);
            free(my_name);
            perror("pwd: path realloc");
            return FAILURE;
        }
    }

    // Add my name to path
    strcat(*path, my_name);
    free(my_name);

    // Leave off /a/b/c'/'
    if(mip != running->cwd)
        strcat(*path, "/");

    iput(mip);
    return SUCCESS;
}
