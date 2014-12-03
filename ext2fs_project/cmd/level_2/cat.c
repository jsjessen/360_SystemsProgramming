#include <cmd.h>
#include <file.h>

int my_cat(int argc, char* argv[])
{
    const int device = running->cwd->device;
    const int block_size = get_block_size(device);

    if(argc < 2)
    {
        fprintf(stderr, "cat: missing operand\n");
        return FAILURE;
    }

    // cat each file given by user
    int i = 1;
    while(i < argc)
    {
        char* pathname = argv[i];

        int fd = open_file(pathname, RD);

        int n = 0;
        char buf[block_size + 1];
        while((n = read_file(fd, buf, block_size)))
        {
            buf[n] = '\0'; // null terminated string

            int j = 0;
            while(buf[j])
            {
                putchar(buf[j]);

                if(buf[j] == '\n')
                    putchar('\r');

                j++;
            }
        } 
        printf("\n\r");

        close_file(fd);
        i++;
    }

    return SUCCESS;
}
