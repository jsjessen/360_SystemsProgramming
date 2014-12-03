#include <cmd.h>

int my_pfd(int argc, char* argv[])
{
    putchar('\n');
    printf("  fd   mode   offset   device   inode \n");
    printf("  --   ----   ------   ------   ----- \n");

    for(int i = 0; i < NOFT; i++)
    {
        OPEN_FILE* ofp = &OpenFileTable[i];

        if(ofp->refCount == 0)
            continue;

        printf("  %02d    ", i);

        switch(ofp->mode)
        {
            case RD:
                printf("RD");
                break;

            case WR:
                printf("WR");
                break;

            case RW:
                printf("RW");
                break;

            case APPEND:
                printf("AP");
                break;

            default:
                printf("??");
                break;
        }

        printf("    %06d     %02d     %05d\n", 
                ofp->offset, ofp->mip->device, ofp->mip->ino);
    }
    putchar('\n');

    return SUCCESS;
}
