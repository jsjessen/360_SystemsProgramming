#include <stdio.h>                                                
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>                    
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h> 

#define FILE_PERM 0644

int run_showblock(char* program, char* disk, char* pathname, char* outfile)
{
    int pid = 0;
    int status = 0;

    // Fork Process
    if((pid = fork()) < 0)
    {
        perror("forking process");
        exit(1);
    }

    if(pid)
    {
        // Parent
        while(waitpid(pid, &status, WNOHANG) == 0)
        {
            sleep(100);
            putchar(' ');
        }
    }
    else
    {
        // Child
        printf("Process(%d) running: %s\n", pid,  program);

        // redirect stdout
        close(1);
        if(open(outfile, O_WRONLY | O_CREAT, FILE_PERM) < 0)
        {
            perror("open outfile");
            exit(1);
        }

        execlp(program, program, disk, pathname, (char*)NULL);
        exit(0);
    }

    return 0;
}

int compare_showblock(char* correct, char* testing)
{
    return 0;
}

int main()
{
    fprintf(stderr, "SHOW ME SOMETHING\n");
    int i = 0;

    char* disk = "fdimage";

    char* my_prog = "showblock";
    char* kc_prog = "showblock";      // CHANGE THIS

    char* my_outfile= "my_outfile";
    char* kc_outfile = "kc_outfile";

    // List of files used for testing
    char* pathname[] = 
    {
        "Z/hugefile",
        (char*)NULL
    };

    printf("Testing begins...\n");

    while(pathname[i])
    {
        int result;
        
        printf("Running: %s %s %s\n", my_prog, disk, pathname[i]);
        run_showblock(my_prog, disk, pathname[i], my_outfile);

        printf("Running: %s %s %s\n", kc_prog, disk, pathname[i]);
        run_showblock(kc_prog, disk, pathname[i], kc_outfile);

        result = compare_showblock(kc_outfile, my_outfile);

        if(result > 0)
        {
            printf("FAILURE\n");
            printf("Disk: %s\n", disk);
            printf("Pathname: %s\n", pathname[i]);
        }
        else
            printf("PASS\n");

        i++;
    }

    return 0;
}
