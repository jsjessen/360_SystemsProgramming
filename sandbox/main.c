#include <stdio.h>

int main(int argc, char* argv[], char* env[])
{
    char cmd[256];
    char filename[256];
    int size;
    int i = 0;

    while(argv[i])
    {
        printf("argv[%d] = %s\n", i, argv[i]);
        i++;
    }

    scanf("%s %s %d", cmd, filename, &size);
    printf("%s %s %d\n", cmd, filename, size);

    return 0;
}
