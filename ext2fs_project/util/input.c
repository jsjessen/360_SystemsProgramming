// James Jessen
// 10918967   
// 
// CptS 360

#include "input.h"

static const int INITIAL_BUF_SIZE = 64;


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

            while (isspace(c) && c != '\n')  
                c = getchar();

            ungetc(c, stdin);

            if(c != '\n')
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

int parse_path(const char* path, char** dirname, char** basename)
{
    int i = 0;
    int c = 0;

    int dirname_start  = 0;
    int dirname_end    = 0;
    int dirname_len    = 0;

    int basename_start = 0;
    int basename_end   = 0;
    int basename_len   = 0;

    if(path == NULL)
        return 0;

    // From the end moving toward start, 
    // finds the first character of a filename
    i = strlen(path);
    while(--i >= 0) // skips null char comparison
    {
        c = path[i];

        if(c != '/' && !isspace(c)) 
            break;
    }
    basename_end = i + 1; // just after it

    // From the last position moving toward start, 
    // finds the first '/'
    i = basename_end;
    while(--i >= 0) 
    {
        c = path[i];

        if(c == '/')
            break;
    }
    basename_start = i + 1; // just after it

    dirname_start = 0;
    dirname_end = basename_start; // end of dirname is the start of basename

    if(strcmp(path, ".") == 0 || strcmp(path, "..") == 0)
        dirname_end = strlen(path);

    if(dirname_end  == 0) dirname_end  = 1;

    if(basename_end == 0) basename_end = 1;

    basename_len = basename_end - basename_start;
    dirname_len  = dirname_end  - dirname_start; 

    if((*dirname  = (char*)malloc((dirname_len  + 1) * sizeof(char))) == NULL)
    {
        perror("input.c: parse_path(): malloc dirname");
        return 0;
    }
    if((*basename = (char*)malloc((basename_len + 1) * sizeof(char))) == NULL)
    {
        perror("input.c: parse_path(): malloc basename");
        return 0;
    }

    if(dirname_end == 1 && path[0] != '/') 
        strcpy(*dirname,  ".");
    else
        strncpy(*dirname,  path + dirname_start,  dirname_len);

    if(strcmp(path, "/") == 0)
        strcpy(*basename, "/");
    else
        strncpy(*basename, path + basename_start, basename_len);

    (*dirname)[dirname_len] = 0;
    (*basename)[basename_len] = 0;

    return 0;
}
