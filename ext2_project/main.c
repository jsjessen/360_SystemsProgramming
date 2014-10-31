/* James Jessen
 * 10918967   
 *
 * CptS 360
 */

#include <stdio.h>

#include "type.h"
#include "input.h"
#include "print.h"
#include "cmd.h"

PROC P[NPROC];
PROC *running;
MINODE minode[NMINODES];
MINODE *root;

int fd;
char buf[1024];
int dev;
char *rootdev;
char *pathname;
char *parameter;

// Initialize data structures of LEVEL-1
void init(void)
{
    int i;

    // (1). 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0 fd[] = 0
    P[0].uid = 0;    
    P[1].uid = 1;

    for (i = 0; i < NPROC; i++)
        P[i].cwd = NULL;

    running = &P[0];

    // circular linked list
    P[0].next = &P[1];
    P[1].next = &P[0];

    for(i = 0; i < NMINODES; i++)
    {
        MINODE* mip = &minode[i];

        mip->dev = mip->ino = 0;
        mip->refCount = 0;
        mip->mounted = 0;
        mip->mptr = 0;
    }

    for(i = 0; i < NPROC; i++)
    {
        int j;

        PROC* p = &P[i];

        p->pid = i;
        p->uid = 0;
        p->cwd = 0;
        p->status = FREE;
        for(j = 0; j < NFD; j++)
            p->fd[j] = 0;
    }

    root = 0;
}

// load root INODE and set root pointer
int mount_root(int dev)
{
    root = iget(dev, 2);
}

int main(int argc, char* argv[])
{
    int   fd;
    char* device;

    if (argc != 2)
    {
        printf("Usage:\n");
        printf("         myext2   DEVICE\n");
        printf("  e.g.   myext2   mydisk\n\n");
        exit(1);
    }

    device = argv[1];

    if((fd = open(device, O_RDWR)) < 0)
    {
        perror("open device");
        exit(1);
    }

    if(!isExt2(fd))
    {
        fprintf(stderr, "%s does not use the ext2 filesystem\n", device);
        exit(1);
    }

    print_super(fd);
    print_gd(fd);
    print_inode(fd, 2);

    getchar();

    init(); // initialize filesystem data structures

    mount_root(fd);

    while(1)
    {
        char* input = NULL;

        int cmd_argc = 0;
        char** cmd_argv = NULL;
        int (*cmd_fptr)(int, char**); 

        do { printf("command : "); }            // Prompt user for input
        while((input = get_input()) == NULL);   // Get user input

        cmd_argv = parse(input, " ");           // Parse input to get cmd argv[]

        while(cmd_argv[cmd_argc + 1])           // Determine cmd argc
        { cmd_argc++; } 

        cmd_fptr = get_cmd(cmd_argv[0]);        // Get the cmd's function pointer
        cmd_fptr(cmd_argc, cmd_argv);           // Execute 

        free(input);
        free_array(cmd_argv);
    }

    return 0;
}
