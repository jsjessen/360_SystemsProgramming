/***************** pipe.c file ***********************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int pd[2];
char buf[128];

main(int argc, char *argv[], char *env[])
{
    int r, n, pid;
    int i; int c;

    r = pipe(pd);  // system call to create a pipe' pd[0]=READ, pd[1]=WRITE
    printf("pd=[%d, %d]\n", pd[0], pd[1]);

    pid = fork();    // fork a child process, which has the SAME pd[ ]

    if (pid){  // parent as pipe READER 
        printf("parent %d read from pipe\n", getpid());
        close(pd[1]); // READER MUST close pd[1]
        close(0);  
        dup(pd[0]);   // replace 0 with pd[0]

        while( (c = getc(stdin)) != EOF){
            putchar(c);
        }

        printf("---------------------------------------\n");
        printf("parent: no more\n");


    }
    else{            // child as pipe WRITER 
        printf("child %d write to pipe\n", getpid());
        close(pd[0]);// WRITER MUST close pd[0]
        close(1);    // close 1
        dup(pd[1]);  // replace 1 with pd[1]

        printf("CHILD WRITES LINES TO PIPE\n");

        for (i=0; i < 10; i++){
            fprintf(stderr, "CHILD WRITES LINE %d TO STDOUT\n", i);
            printf("this is line %d from child\n", i);
        }
    }
}
