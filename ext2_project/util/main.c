#include "input.h"

//  path       &dirname   &basename
    //  -----------------------------
    //  /usr/lib   /usr      lib
    //  /usr/      /         usr
    //  usr        .         usr
    //  /          /         /
    //  .          .         .
    //  ..         .         ..

int main()
{
    int i = 0;
    char* path[] = { "/usr/lib", "/usr/", "usr", "/", ".", ".." };

    int path_count = 6;
    char* dirname = NULL;
    char* basename = NULL;

    for(i = 0; i < path_count; i++)
    {
        puts("=========================");
        parse_path(path[i], &dirname, &basename); 
        puts("-------------------------");
        printf("dirname = %s\n", dirname);
        printf("basename = %s\n", basename);
    }

    return 0;
}

