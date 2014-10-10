/*************** a.c file **************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pid;

char line[128];
char* const token[] = {"one", "two", "Free", NULL};
char cwd[128], command[128];

main(int argc, char *argv[ ])
{
    // 1. DRAW A DIAGRAM TO SHOW EXACTLY WHAT IS argv:

    // argv[0] -> "b.out"
    // argv[1] -> "one"
    // argv[2] -> "two"
    // etc...

    // DEFINE myargv as AN ARRAY OF 32 char pointers:
    //     _____????_____ myargv___???____;
    //char* myargv[32];

    // 2. Write code to           |-------- n=5--------| 
    //    input a line, e.g.    b.out this is a test
    //    tokenize line into inidividual strings as 
    //    token[0], token[1],....,token[n-1]

    int i = 0;
    /*char* tok;

    printf("Enter line: ");
    fgets(line, 125, stdin);

    printf("Line = %s\n", line);

    tok = strtok(line, " \t\n");
    while(tok != NULL)
    {
        int length = strlen(tok); // +2 for null char and '/'
        token[i] = (char*)calloc(1, length + 1);
        strcpy(token[i], tok);
        tok = strtok(NULL, " \t\n");
        i++;
    }
    token[i] = NULL;
*/
    // Print argv
    i = 0;
    while(token[i] != NULL)
    {
        printf("token[%d] = %s\n", i, token[i]);
        i++;
    }


    // 3. Write code to let myargv[i] point to token[i], (0 <= i < n)
    //    myargv[n] = 0;
    /*
    while(token[i] != NULL)
    {
        myargv[i] = token[i];
    }
    */

    pid = getpid();
    printf("proc %d in a.out exec to b.out\n", pid);

    getcwd(cwd,128);     // cwd contains the pathname of CWD  
    printf("cwd = %s\n", cwd);

    // WRITE CODE to let command = CWD/b.out
    strcpy(command, cwd);
    strcat(command, "/b.out");

    printf("command= %s\n", command);

    execve("/home/jsjessen/Documents/360_SystemsProgramming/3prelab/b.out", token);

    perror("Error");

    printf("execve failed\n");
}
