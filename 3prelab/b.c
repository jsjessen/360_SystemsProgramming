/***************** b.c file ************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main(int argc, char *argv[ ])
{
    int i = 0;
    printf("proc %d in b.out: argc=%d\n", getpid(), argc);

    // Print argv
    while(argv[i] != NULL)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
        i++;
    }

    printf("proc %d in b.out exit\n", getpid());
}
