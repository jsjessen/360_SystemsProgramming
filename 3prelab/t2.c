/******************** t2.c *********************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *p = 0;

main()
{
    int pid, status;
    int exitValue;

    printf("PROC %d forks a child\n", getpid());

    pid = fork();

    if (pid){ // PARENT:
        printf("\nPARENT %d WAITS FOR CHILD %d TO DIE\n", getpid(),pid);
        pid=wait(&status);
        printf("Exited with %u\n", WEXITSTATUS(status));
        printf("Signal was %u\n", WTERMSIG(status));
        printf("Parent %d: DEAD CHILD=%d, status=%04x\n", getpid(), pid, status);
    }
    else{// child:
        printf("this is child proc %d, input a value (0-255) to die: ", getpid());
        scanf("%d", &exitValue);     
        printf("child %d dies by exit(%d)\n", getpid(), exitValue);
//HERE:
        //{ int a,b; a = 1; b = 0; a = a/ b; }
        //*p = 1234;
        //fprintf(stderr, "ERROR\n");

        exit(exitValue); 

    }
}
