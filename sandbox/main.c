#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

//#define u8 unsigned char
typedef unsigned char u8;

int main(int argc, char* argv[], char* env[])
{
    u8 byte = 3;

    printf("%d \n", byte);
}
