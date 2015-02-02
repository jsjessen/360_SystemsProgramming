#include "myprint.h"

int main()
{
    myprintf("|Testing: %d,%d,%d|\n\n", 1,2,3);
    myprintf("Testing: %c|%s|%d|%o|%x|%u\n\n", 'J',"Hello",-13,052,0x2F,42); 

    return 0;
}
