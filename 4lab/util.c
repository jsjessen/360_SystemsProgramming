#include "util.h"

void free_array(char** array)
{
    int i = 0;

    if(!array)
        return;

    while(array[i])
        free(array[i++]);

    free(array);
}
