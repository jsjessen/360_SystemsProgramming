/* James Jessen
 * 10918967   
 *
 * CptS 360
 * Lab #3
 */

#include "input.h"

char* get_input()
{
    int i = 0;
    int size = 16;
    char* buf;

    if((buf = (char*)malloc(size)) == NULL)
    {
        perror("input.c: initial buf malloc");
        return NULL;
    }
    buf[0] = 0;

    // Loop through input 1 char at a time
    while(1)
    {
        int c = getchar();

        // Increase buffer size as needed
        if((i + 1) == size) 
        { 
            size += size;

            // Double Size
            if((buf = (char*)realloc(buf, size)) == NULL)
            {
                perror("input.c: buf realloc");
                return NULL;
            }
        }

        // Accept input or if no input, allow user to cycle
        if (c == '\n')
        {
            if (buf[0] == 0)
                return NULL;
            else
                break;
        }

        // Replace block of whitespace with single space
        if (isspace(c))
        {
            // Ignore leading whitespace
            if (buf[0] == 0)
                continue;

            while (isspace(c))  
                c = getchar();

            ungetc(c, stdin);
            buf[i++] = ' ';
            continue;
        }

        buf[i++] = c;
    }
    buf[i] = 0; // Append with null char

    return buf;
}
