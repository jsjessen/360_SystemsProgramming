/* James Jessen
 * 10918967   
 *
 * CptS 360
 * Lab #3
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "input.h"
#include "shell.h"

int main(int argc, char* argv[], char* env[])
{
    char** paths = parse(search_env(env, "PATH"), ":");
    char* line = NULL;

    while(1)
    {
        int pid, status;

        free(line);

        // Get user input 
        do { printf("jjsh %% : "); }
        while((line = get_input()) == NULL);

        if(strncmp(line, "exit", strlen("exit")) == 0)
            break;

        if(strncmp(line, "cd", strlen("cd")) == 0)
        {
            my_cd(line + strlen("cd "), search_env(env, "HOME"));
            continue;
        }

        // Fork Process
        if((pid = fork()) < 0)
            perror("main.c: jjsh forking child process");

        if(pid)
        {
            // Parent 
            printf("jjsh waits\n");
            pid = wait(&status);
            printf("%d died : exit status = %04x\n", pid, status);
        }
        else
        {
            // Child 
            printf("jjsh forks child process %d\n", getpid());

            // Attempt to execute Unix Command
            process_line(line, paths, env);
        }
    }

    free(line);
    free_array(paths);
    return 0;
}
