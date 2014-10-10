#include "shell.h"

char* search_env(char* env[], char* target)
{
    int length = strlen(target);
    int i = 0;

    while (env[i])
    {
        if (strncmp(target, env[i], length) == 0)
            return (env[i] + length + 1);

        i++;
    }
    return NULL;
}

void free_array(char** array)
{
    int i = 0;

    if(!array)
        return;

    while(array[i])
        free(array[i++]);

    free(array);
}

int my_cd(char* arg1, char* home)
{
    char* new_cwd;

    if(arg1)
        new_cwd = arg1; 
    else
        new_cwd = home;

    printf("%d cd to %s ", getpid(), new_cwd);

    if(chdir(new_cwd) != 0)
    {
        printf("FAILED!\n");
        perror("shell.c: my_cd(): chdir");
        return -1;
    }
    printf("OK\n");

    return 0;
}

void io_redirect(char** my_argv)
{
    int i = 1; // my_argv[0] is cmd, skip it
    while (my_argv[i])
    {
        if (strcmp(my_argv[i], "<") == 0)
        {
            // Replace stdin with file
            close(0); 
            if(open(my_argv[i + 1], O_RDONLY) < 0)
                perror("shell.c: do_command(): < open");

            my_argv[i] = NULL; // Hide "< File" from executing command 
            return;
        }

        if (strcmp(my_argv[i], ">") == 0)
        {
            // Replace stdout with file
            close(1);
            if(open(my_argv[i + 1], O_WRONLY | O_CREAT, FILE_PERM) < 0)
                perror("shell.c: do_command(): > open");

            my_argv[i] = NULL; // Hide "> File" from executing command 
            return;
        }

        if (strcmp(my_argv[i], ">>") == 0)
        {
            // Replace stdout with file (append)
            close(1);
            if(open(my_argv[i + 1], O_WRONLY | O_APPEND | O_CREAT, FILE_PERM) < 0)
                perror("shell.c: do_command(): >> open");

            my_argv[i] = NULL; // Hide ">> File" from executing command 
            return;
        }
        i++;
    }
}

// Using fprintf(stderr) in place of printf()
// to avoid this output getting mixed up in 
// IO redirection and Piping
void process_line(char* line, char** paths, char* env[])
{
    if(!line || (line[0] == 0) || (strcmp(line, "") == 0))
        exit(0);

    fprintf(stderr,"%d process_line: line=\"%s\"\n", getpid(), line);

    int pd[2];
    int pid, status;

    // Create a copy of line for this process to use safely
    // line = A | B | C
    // head = A
    // tail = B | C
    int length = strlen(line);
    char* head = strtok(line, "|");
    char* tail = NULL;

    if(strlen(head) < length)
    {
        // head|tail0
        // .....^
        tail = line + strlen(head) + 1;
    }
    else
    {
        // head0
        // ....^
        tail = line + length;
    }

    fprintf(stderr,"%d head=\"%s\"  tail=\"%s\"\n", getpid(), head, tail);

    // If no tail, just do the command in head
    if(!tail || (strcmp(tail, "") == 0))
        do_command(head, paths, env);

    // Create Pipe
    if(pipe(pd) != 0)
        perror("shell.c: process_line(): creating pipe");

    // Fork child process
    if((pid = fork()) < 0)
        perror("shell.c: process_line(): forking child process");

    if(!pid)
    {  
        // Child - Pipe WRITER
        fprintf(stderr,"%d forks a child process %d\n", getppid(), getpid());

        close(pd[0]);// writer MUST close PipeIn 
        close(1);    // close stdout 
        dup(pd[1]);  // replace stdout with PipeOut 

        // Head is always atomic cmd, attempt to execute it
        do_command(head, paths, env);
    }
    else
    {            
        // Parent - Pipe READER
        fprintf(stderr,"%d waits\n", getpid());
        pid = wait(&status);
        fprintf(stderr,"%d died : exit status = %04x\n", pid, status);

        close(pd[1]); // reader MUST close PipeOut
        close(0);     // close stdin
        dup(pd[0]);   // replace stdin with PipeIn

        // Tail could contain more pipes
        // Recurse, only head runs commands
        process_line(tail, paths, env);
    } 
}

// Using fprintf(stderr) in place of printf()
// to avoid this output getting mixed up in 
// IO redirection and Piping
void do_command(char* line, char** paths, char* env[])
{
    if(!line || (line[0] == 0) || (strcmp(line, "") == 0))
        exit(0);

    fprintf(stderr,"%d do_command: line=\"%s\"\n", getpid(), line);

    int i = 0;
    char* cmd = NULL;
    char** my_argv = parse(line, " ");

    fprintf(stderr,"%d tries %s in each PATH directory\n", getpid(), my_argv[0]);

    while(paths[i])
    {
        int length = strlen(paths[i]) + 1 + strlen(my_argv[0]); // +1 for '/'

        free(cmd);
        cmd = (char*)malloc(length + 1); // +1 for null char

        strcpy(cmd, paths[i]);
        strcat(cmd, "/");
        strcat(cmd, my_argv[0]);
        fprintf(stderr,"PATH[%d] Command=%s\n", i, cmd);

        // Check that file exists and is executable
        if(access(cmd, F_OK | X_OK) < 0)
        {
            i++;
            continue;
        }
        else
        {
            // Check for IO redirection (<, >, >>)
            io_redirect(my_argv);
            break;
        }
    }

    fprintf(stderr,"--------------------------------------------------\n");

    // Execute the command
    execve(cmd, my_argv, env);

    // Getting here implies execution failed
    fprintf(stderr,"Invalid Command: %s\n", my_argv[0]);
    free(cmd);
    free_array(my_argv);
    exit(0);
}
