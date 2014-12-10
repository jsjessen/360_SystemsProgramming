#include <cmd.h>

int my_read(int argc, char* argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "read: missing operand\n");

        printf("\nUsage:\n");
        printf("        read FileDescriptor NumberOfBytes\n");
        printf("  e.g.  read 3 1024\n");

        // Show open fd 
        my_pfd(0, NULL);

        return MISSING_OPERAND;
    }

    char* fd_str = argv[1];
    const int fd = atoi(fd_str);

    char* nbytes_str = argv[2];
    const int nbytes = atoi(nbytes_str);

    char buf[nbytes + 1];
    int bytes_read = read_file(fd, buf, nbytes);
    buf[bytes_read] = '\0'; // null terminated string

    printf("\nRead %d bytes from file descriptor %d\n", bytes_read, fd);
    print_divider('-');
    puts(buf);
    print_divider('-');

    my_pfd(0, NULL);

    return bytes_read;
}
