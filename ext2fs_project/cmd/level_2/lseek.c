#include <cmd.h>

int my_lseek(int argc, char* argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "lseek: missing operand\n");

        printf("\nUsage:\n");
        printf("        lseek FileDescriptor Position\n");
        printf("  e.g.  lseek 3 214\n");

        // Show open fd 
        my_pfd(0, NULL);

        return MISSING_OPERAND;
    }

    char* fd_str = argv[1]; 
    int fd = atoi(fd_str); 

    char* position_str = argv[2];
    int position = atoi(position_str); 

    position = seek_file(fd, position);

    my_pfd(0, NULL);

    return position;
}

