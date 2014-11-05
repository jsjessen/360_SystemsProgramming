#include <stdio.h>                                                
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>                    
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h> 

#define FILE_PERM 0644
#define MAX_NUM_GETCHARS 100

int run_showblock(char* program, char* disk, char* pathname, char* outfile)
{
    int i = 0;
    int pd[2];
    int pid = 0;
    int status = 0;

    printf("Running: %s %s %s > %s\n", program, disk, pathname, outfile);

    // Create Pipe
    if(pipe(pd) != 0)
        perror("run_showblock(): creating pipe");

    // Fork child process
    if((pid = fork()) < 0)
        perror("run_showblock(): forking child process");

    if(pid)
    {       // Parent - Pipe WRITER

        close(pd[0]);// writer MUST close PipeIn 
        close(1);    // close stdout 
        dup(pd[1]);  // replace stdout with PipeOut 

        // So the getchar()'s in program don't halt execution
        for(i = 0; i < MAX_NUM_GETCHARS; i++)
            putchar('\n');

        return pid;
    }
    else
    {       // Child - Pipe READER         

        close(pd[1]); // reader MUST close PipeOut
        close(0);     // close stdin
        dup(pd[0]);   // replace stdin with PipeIn

        // Redirect stdout to outfile
        close(1);
        if(open(outfile, O_WRONLY | O_CREAT, FILE_PERM) < 0)
            perror("open outfile");

        fprintf(stderr, "Writing to %s\n", outfile);

        execlp(program, program, disk, pathname, (char*)NULL);
        fprintf(stderr, "Execution of %s FAILED\n", program);
        exit(1);
    } 
}

int compare_showblock(char* testing, char* correct)
{
    printf("Comparing: %s to %s\n", testing, correct);
    return 0;
}

int main()
{
    char* disk = "fdimage";

    int program_count = 2;
    char* program[] = { "my_showblock", "kc_showblock.bin" };
    char* outfile[] = { "my_outfile"  , "kc_outfile"       };

    // List of files used for testing
    char* pathname[] = 
    {
        "tiny",
        "X/tiny",
        "Y/bigfile",
        "Z/hugefile",
        "dontExist",
        "How do you fail?",
        (char*)NULL
    };

    int i = 0;
    while(pathname[i])
    {
        int j;
        int pid; 
        int status;
        int result;

        printf("\n----------------------------------------------\n\n");

        for(j = 0; j < program_count; j++)
        {
            // Remove outfile so it guaranteed to be empty
            remove(outfile[j]);

            // Fork Process
            if((pid = fork()) < 0)
                perror("main(): forking child process");

            if(pid)
            {
                // Parent 

                // Wait for immediate child process to die
                waitpid(pid, &status, 0);
            }
            else
            {
                // Child 
                int child_pid;

                child_pid = run_showblock(program[j], disk, pathname[i], outfile[j]);

                // Wait for children processes to die
                waitpid(child_pid, &status, 0);
                exit(0);
            }
            putchar('\n');
        }

        result = compare_showblock(outfile[0], outfile[1]);

        if(result > 0)
        {
            printf("\n*********************\n");
            printf("FAILED \n");
            printf("Disk: %s\n", disk);
            printf("Pathname: %s\n", pathname[i]);
        }
        else
            printf("PASSED\n");

        i++;
    }

    printf("\n----------------------------------------------\n\n");
    return 0;
}
