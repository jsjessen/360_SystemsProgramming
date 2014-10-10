/*************** a.c file **************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int pid;

char cwd[256], command[256];

int main(int argc, char *argv[ ])
{
    int rc = 0;
    char * const myargv[] = {"one", "two", "Free", NULL};
    char * const envp[] = {NULL};

    pid = getpid();
    printf("proc %d in a.out exec to b.out\n", pid);

    getcwd(cwd,128);     // cwd contains the pathname of CWD  
    printf("cwd = %s\n", cwd);

    // WRITE CODE to let command = CWD/b.out
    strcpy(command, cwd);
    strcat(command, "/b.out");

    printf("command= %s\n", command);

    rc = execve(command, myargv, envp);

    perror("execve ERROR");

    return rc;
}
