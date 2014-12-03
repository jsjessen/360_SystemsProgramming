#include <cmd.h>

int my_write(int argc, char* argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "write: missing operand\n");

        printf("\nUsage:\n");
        printf("        write FileDescriptor \"TextToWrite\"\n");
        printf("  e.g.  write 3 This text will be written to the file.\n");

        // Show open fd 
        my_pfd(0, NULL);

        return FAILURE;
    }

    char* fd_str = argv[1];
    const int fd = atoi(fd_str);

    int i = 2;
    char* buf = (char*)malloc((strlen(argv[i]) + 1) * sizeof(char*));
    strcpy(buf, argv[i++]);
    while(i < argc)
    {
        buf = (char*)realloc(buf, (strlen(buf) + strlen(argv[i]) + 2) * sizeof(char));
        strcat(buf, " ");
        strcat(buf, argv[i++]);
    }
    int nbytes = strlen(buf); 

    int bytes_wrote = write_file(fd, buf, nbytes);

    printf("\nWrote %d bytes to file descriptor %d\n", bytes_wrote, fd);
    print_divider('-');
    puts(buf);
    print_divider('-');

    my_pfd(0, NULL);

    return bytes_wrote;
}
