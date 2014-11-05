#include "compare.h"

typedef enum { false, true } bool;
typedef enum { BLOCK_START, NUMBERS, OTHER } line_type;

static char* DELIMS = " -=*\t\n";


int get_line_type(char* line_ptr)
{ 
    int i = 0;
    int word_len = 0;
    int direct_len = 0;
    char** words = parse(line_ptr, DELIMS);

    if(!words || !words[0])
        return OTHER;

    // First and last work begin with a digit/number
    if(isdigit(words[0][0]) && isdigit(words[i][0]))
        return NUMBERS;

    while(words[i + 1]) 
    { 
        int j;
        for(j = 0; words[i][j]; j++)
            words[i][j] = tolower(words[i][j]);

        //printf("%s ", words[i]);
        i++; 
    }
    //putchar('\n');
    if(i < 1)
        return OTHER;

    word_len = strlen(words[i - 1]);
    direct_len = strlen("direct");

    if(i < 1 || word_len < direct_len)
        return OTHER;

    // Ends with "...direct X", as in "direct blocks", "indirect blocks", etc...
    if(strcmp(&words[i - 1][word_len - direct_len], "direct") == 0) 
        return BLOCK_START;

    return OTHER;
}


int line_compare(char* line_ptr1, char* line_ptr2)
{    
    int i = 0;
    int diff_count = 0;
    char** words1 = parse(line_ptr1, DELIMS);
    char** words2 = parse(line_ptr2, DELIMS);
    int length1 = 0;
    int length2 = 0;

    if(!words1 || !words2)
        return -1;

    if(!words1[0] && !words2[0])
        return 0;

    while(words1[i] && words2[i])
    {
        if(strcmp(words1[i], words2[i]) != 0)
            diff_count++;

        i++;
    }

    while(words1[length1++]) {}
    while(words2[length2++]) {}

    if(length1 > length2)
        diff_count += (length1 - length2);
    else
        diff_count += (length2 - length1);

    return diff_count;
}


int file_compare(char* filename1, char* filename2, int* err_line1, int* err_line2)
{
    FILE* fp1 = NULL;
    FILE* fp2 = NULL;

    char* line_ptr1 = NULL;
    char* line_ptr2 = NULL;

    size_t len1 = 0;
    size_t len2 = 0;

    ssize_t read1;
    ssize_t read2;

    int diff_count = 0;

    int line_count1 = 0;
    int line_count2 = 0;

    *err_line1 = 0;
    *err_line2 = 0;

    // Open files for read
    if((fp1 = fopen(filename1, "r")) == NULL)
    {
        fprintf(stderr, "Unable to open: %s\n", filename1);
        exit(EXIT_FAILURE);
    }
    if((fp2 = fopen(filename2, "r")) == NULL)
    {
        fprintf(stderr, "Unable to open: %s\n", filename2);
        exit(EXIT_FAILURE);
    }

    // Get both to first block
    while(((read1 = getline(&line_ptr1, &len1, fp1)) != -1) &&
            (get_line_type(line_ptr1) != BLOCK_START)) { line_count1++; }

    while(((read2 = getline(&line_ptr2, &len2, fp2)) != -1) &&
            (get_line_type(line_ptr2) != BLOCK_START)) { line_count2++; }

    while(true)
    {
        int new_diff = 0;

        // Keep going until hit a NUMBERS line
        while(((read1 = getline(&line_ptr1, &len1, fp1)) != -1) &&
                (get_line_type(line_ptr1) != NUMBERS)) { line_count1++; }

        while(((read2 = getline(&line_ptr2, &len2, fp2)) != -1) &&
                (get_line_type(line_ptr2) != NUMBERS)) { line_count2++; }

        // If one file is at the end, the remainder of the other is different
        if(read1 == -1)
        {
            while((read2 = getline(&line_ptr2, &len2, fp2)) != -1)
            {
                diff_count++;
                puts(line_ptr2);
            }
            break;
        }
        if(read2 == -1)
        {
            while((read1 = getline(&line_ptr1, &len1, fp1)) != -1)
            {
                diff_count++;
                puts(line_ptr1);
            }
            break;
        }

        if((new_diff = line_compare(line_ptr1, line_ptr2)) > 0)
        {
            if(*err_line1 == 0)
                *err_line1 = line_count1;

            if(*err_line2 == 0)
                *err_line2 = line_count2;

            diff_count += new_diff;
            printf("%s: %s", filename1, line_ptr1);
            printf("%s: %s\n", filename2, line_ptr2);
        }
    }

    free(line_ptr1);
    free(line_ptr2);

    fclose(fp1);
    fclose(fp2);

    return diff_count;
}
