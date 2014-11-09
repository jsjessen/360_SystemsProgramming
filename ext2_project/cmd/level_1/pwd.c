#include <cmd.h>

int recursive_pwd(MINODE* mip, char** path, int size)
{
    int ino = 0;
    char* myname = NULL;

    MINODE* parent = NULL;

    if(mip == root)
    {
        iput(mip);
        strcat(*path, "/");
        return 0;
    }

    // Find my inode number and parent MINODE
    findino(mip, &ino, parent);
    iput(mip);

    // Recurse
    if(recursive_pwd(parent, path, size) != 0)
    {
        iput(parent);
        return -1;
    }

    // Add name of current dir
    findmyname(parent, ino, &myname);

    // Curent + New + / + null + root(/)
    while(strlen(*path) + strlen(myname) + 3 >= size)
    {
        size += size;

        // Double Size
        if((*path = (char*)realloc(*path, size * sizeof(char))) == NULL)
        {
            perror("pwd: path re-allocation");
            return -1;
        }
    }

    // Add name of current dir to path
    strcat(*path, myname);
    strcat(*path, "/");

    iput(parent);
    free(myname);
    return 0;
}

// print the pathname of current working directory 
int my_pwd(int argc, char* argv[])
{
    int initial_size = 256;
    char* path = NULL;

    // Allocate memory for the path of the cwd
    if((path = (char*)malloc(initial_size * sizeof(char*))) == NULL)
    {
        perror("pwd: path initial allocaiton");
        return 0;
    }

    running->cwd->refCount++;
    recursive_pwd(running->cwd, &path, initial_size);
    puts(path);

    free(path);
    return 1;
}
