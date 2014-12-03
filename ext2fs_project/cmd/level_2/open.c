#include <cmd.h>

int my_open(int argc, char* argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "open: missing operand\n");

        printf("\nUsage:\n");
        printf("        open FILE   MODE\n");
        printf("  e.g.  open myfile RD\n");

        printf("\nOptions:\n");
        printf("        RD WR RW APPEND\n\n");

        return FAILURE;
    }

    char* pathname = argv[1];
    char* mode_str = argv[2];

    int mode = 0;
    if(strcmp(mode_str, "RD") == 0)
        mode = RD;
    else if(strcmp(mode_str, "WR") == 0)
        mode = WR;
    else if(strcmp(mode_str, "RW") == 0)
        mode = RW;
    else if(strcmp(mode_str, "APPEND") == 0)
        mode = APPEND;
    else
    {
        fprintf(stderr, "open: unknown mode '%s'\n", mode_str);
        return FAILURE;
    }

    return open_file(pathname, mode);
}
