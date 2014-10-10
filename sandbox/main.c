#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[], char* env[])
{
    fprintf(stderr,"Just before fork\n");
    if(fork() == 0)
    {
        fprintf(stderr,"exe\n");
        execve(argv[0], argv, env);
        perror("exe");
    }
    fprintf(stderr,"Just before wait\n");
    wait(&argc);
    perror("wait");
    fprintf(stderr,"argc=%d\n", argc);
}
