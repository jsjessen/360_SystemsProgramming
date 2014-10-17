#include "util.h"

int get_block(int fd, int block, char *buf)
{
    lseek(fd, (long)(block * BLK_SIZE), 0);
    read(fd, buf, BLK_SIZE);
}


