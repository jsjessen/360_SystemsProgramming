// James Jessen
// 10918967   
// 
// CptS 360

#include "input.h"

static const int INITIAL_BUF_SIZE = 8;


// Get input from user in a controlled manner
char* get_input()
{
    int i = 0;
    int size = INITIAL_BUF_SIZE;
    char* buf;

    if((buf = (char*)malloc(size)) == NULL)
    {
        perror("input.c: get_input(): initial buf malloc");
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
                perror("input.c: get_input(): buf realloc");
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

// Parse input string by splitting on delimiters
char** parse(const char* input, const char* delimiters)
{
    int i = 0;
    int size = INITIAL_BUF_SIZE;
    char* copy;
    char* tok;
    char** buf;

    if((copy = (char*)malloc((strlen(input) + 1) * sizeof(char))) == NULL)
    {
        perror("input.c: parse(): copy malloc");
        return NULL;
    }
    strcpy(copy, input);

    if((buf = (char**)malloc(size * sizeof(char*))) == NULL)
    {
        perror("input.c: parse(): initial ouput malloc");
        return NULL;
    }

    if((tok = strtok(copy, delimiters)) == NULL)
        return NULL;

    while(tok)
    {
        int length = strlen(tok) + 1;
        if((buf[i] = (char*)malloc(length)) == NULL)
        {
            perror("input.c: parse(): buf malloc for tok");
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
                perror("input.c: parse(): buf realloc");
                return NULL;
            }
        }

        tok = strtok(NULL, delimiters);
        i++;
    }
    buf[i] = NULL;

    free(copy);
    return buf;
}
