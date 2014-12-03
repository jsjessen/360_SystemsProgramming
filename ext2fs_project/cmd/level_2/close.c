#include <cmd.h>

int my_close(int argc, char* argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "close: missing operand\n");

        printf("\nUsage:\n");
        printf("        close FileDescriptor\n");
        printf("  e.g.  close 3\n");

        // Show open fd 
        my_pfd(0, NULL);

        return FAILURE;
    }

    char* fd_str = argv[1];
    int fd = atoi(fd_str);

    return close_file(fd);
}
