#include <cmd.h>

int rpwd(MINODE* mip, char** path, int size)
{
    int my_ino = 0;
    int parent_ino = 0;
    char* my_name = NULL;
    MINODE* parent_mip = NULL;

    // Base case: check
    if(mip == root)
    {
        iput(mip);
        strcat(*path, "/");
        return 1;
    }

    // Check . and .. in my directory for my/parent's inode numbers 
    findino(mip, &my_ino, &parent_ino);

    // Get my parent directory's MINODE
    parent_mip = iget(running->cwd->dev, parent_ino);

    // Do the same for parent 
    if(!rpwd(parent_mip, path, size))
    {
        iput(mip);
        return 0;
    }

    // path = /a/b/.../<need to add my name here>

    // Look in parent DIR for my name using my inode number
    findmyname(parent_mip, my_ino, &my_name);

    // path + my_name + / + null + root(/)
    while(strlen(*path) + strlen(my_name) + 3 >= size)
    {
        size += size;

        // Double size of path
        if((*path = (char*)realloc(*path, size * sizeof(char))) == NULL)
        {
            iput(mip);
            free(my_name);
            perror("pwd: path realloc");
            return 0;
        }
    }

    // Add my name to path
    strcat(*path, my_name);

    // Leave off /a/b/c'/'
    if(mip != running->cwd)
        strcat(*path, "/");

    iput(mip);
    free(my_name);
    return 1;
}

// print the pathname of current working directory 
int my_pwd(int argc, char* argv[])
{
    int initial_size = 128; // DEBUG WITH LOWER VALUE
    char* path = NULL;

    // Keep things simple if cwd is root
    //if(running->cwd == root)
    //{
    //    printf("/\n");
    //    return 1;
    //}

    // Minimum: root(/) + null (to avoid multiple realloc checks)
    if(initial_size < 2)
        initial_size = 2;

    // Allocate memory for the path of the cwd
    if((path = (char*)malloc(initial_size * sizeof(char*))) == NULL)
    {
        perror("pwd: path initial malloc");
        return 0;
    } 

    // null char for strcat, otherwise it doesn't know location of end
    path[0] = 0;
    path[1] = 0;

    // rpwd assume mip was obtained with iget and iputs accordingly
    running->cwd->refCount++;

    if(rpwd(running->cwd, &path, initial_size))
        puts(path);

    free(path);
    return 1;
}
