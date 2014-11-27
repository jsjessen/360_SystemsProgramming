#include <cmd.h>

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
        u8* end_marker = 0;  // a null char at end of buf[ ]

        int fd = my_open(pathname, R);

        char* buf = NULL;
        while(int n = read(fd, buf, block_size))
        {
            //buf[n] = '\0'; // as a null terminated string

            int j = 0;
            while(buf[j])
            {
                putchar(buf[j]);

                if(buf[j] == '\n')
                    putchar('\r');

                j++
            }
            free(buf);
        } 
        printf("\n\r");

        close(fd);
        i++;
    }
}
