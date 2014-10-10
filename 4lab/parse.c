/* James Jessen
 * 10918967   
 *
 * CptS 360
 * Lab #3
 */

#include "parse.h" 

char** parse(char* input, char* delimiters)
{
    int i = 0;
    int size = 8;
    char* tok;
    char** buf;

    if((buf = (char**)malloc(size * sizeof(char*))) == NULL)
    {
        perror("parse.c: initial ouput malloc");
        return NULL;
    }

    if((tok = strtok(input, delimiters)) == NULL)
        return NULL;

    while(tok)
    {
        int length = strlen(tok) + 1;
        if((buf[i] = (char*)malloc(length)) == NULL)
        {
            perror("parse.c: buf malloc for tok");
            return NULL;
        }
        strcpy(buf[i], tok);

        // Increase buffer size as needed
        if((i+1) == size)
        {
            size += size;

            // Double buffer size
            if((buf = (char**)realloc(buf, size * sizeof(char*))) == NULL)
            {
                perror("parse.c: buf realloc");
                return NULL;
            }
        }

        tok = strtok(NULL, delimiters);
        i++;
    }
    buf[i] = NULL;

    return buf;
}
