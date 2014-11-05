// parser for output of shwowblock
// using parse function from James
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int INITIAL_BUF_SIZE = 8;

char** parse(char* input, char* delimiters)
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

int compare(char *correct, char *testing) { //returns negative value if their is a failure, returns value greater than zero if success
    FILE *kc_file, *test_file;

    char line[256];
    char *kc_blocks[512], *test_blocks[512];
    char *ret;
    char **tokens;	
    int bytes, start=0, i,correct_count = 0, test_count = 0, j = 0, k = 0;
    printf("\nComparing block numbers of the two output files...\n");
    printf("--------------------------------------------------\n");

    if ( !(kc_file = fopen(correct, "r")) ) {
        printf("error opening file: %s\n", correct);
        return 1;
    }
    printf("Parsing file: %s\n\n", correct);
    //start parsing through the first file
    while(fgets(line, sizeof(line), kc_file)) {
        if (start) //start parsing for block numbers
        {
            i = 0;
            tokens = parse(line, " "); //get the tokens of the line
            while(tokens[i]) {
                if ( (strlen(tokens[i]) < 4) && (strlen(tokens[i]) > 1) && (strcmp(tokens[i], "==") != 0) && (strcmp(tokens[i], "=") != 0) && (strcmp(tokens[i], "0\n") != 0) && (strcmp(tokens[i], " ") != 0) && 		(strcmp(tokens[i], "->") != 0)  ) //looking for numbers of length 2 or 3, and the rest is to get rid of garbage that pops up in between
                {
                    if(tokens[i][2] == '\n') //special cases where newline characters where the 2 digit number was at the end of a line (gets rid of the \n)
                        tokens[i][2] = 0;
                    //printf("tokens[i]=%s len: %d\n", tokens[i], strlen(tokens[i]));
                    correct_count++;
                    //printf("kc: %s\t", tokens[i]);
                    kc_blocks[j++] = tokens[i];

                }
                i++;

            }
        }
        //check to see if we are at the block numbers yet
        if (ret = strstr(line, "Direct Blocks"))
            start = 1;


    }
    printf(" correct block count= %d\n", correct_count);
    // close the kc output file
    fclose(kc_file);
    //start parsing the testing file
    start = 0;
    if ( !(test_file = fopen(testing, "r")) ) {
        printf("error opening file: %s\n", correct);
        return 1;
    }
    printf("Parsing file: %s\n\n", testing);
    while(fgets(line, sizeof(line), test_file)) {
        if (start) //start parsing for block numbers
        {
            i = 0;
            tokens = parse(line, " "); //get the tokens of the line
            while(tokens[i]) {
                if ( (strlen(tokens[i]) < 4) && (strlen(tokens[i]) > 1) && (strcmp(tokens[i], "==") != 0) && (strcmp(tokens[i], "=") != 0) && (strcmp(tokens[i], "0\n") != 0) && (strcmp(tokens[i], " ") != 0) && 		(strcmp(tokens[i], "->") != 0)  ) //looking for numbers of length 2 or 3, and the rest is to get rid of garbage that pops up in between
                {
                    if(tokens[i][2] == '\n') //special cases where newline characters where the 2 digit number was at the end of a line (gets rid of the \n)
                        tokens[i][2] = 0;
                    //printf("tokens[i]=%s len: %d\n", tokens[i], strlen(tokens[i]));
                    if (strcmp(tokens[i], "322") != 0) {
                        test_count++;
                        //printf("test: %s\t", tokens[i]);
                        test_blocks[k++] = tokens[i];
                    }

                }
                else if ( (strlen(tokens[i]) == 4) && ((tokens[i][3] == '\n') || (tokens[i][3] == ':'))  ) //add this only for your showblock, special cases with extra \n character
                {
                    tokens[i][3] = 0;
                    if (tokens[i][2] != ':') //special case
                    {
                        //printf("tokens[i]=%s len: %d\n", tokens[i], strlen(tokens[i]));
                        test_count++;
                        //printf("test: %s\t", tokens[i]);
                        test_blocks[k++] = tokens[i];
                    }
                }
                else if ( strlen(tokens[i]) == 5) { //for the 323:
                    tokens[i][3] = 0;
                    test_count++;
                    //printf("test: %s\t", tokens[i]);
                    test_blocks[k++] = tokens[i];
                }
                i++;

            }
        }
        //check to see if we are at the block numbers yet
        if (ret = strstr(line, "Direct Blocks"))
            start = 1;


    }
    printf(" test block count= %d\n\n", test_count);
    if ( (test_count == 0) && (correct_count == 0) )
    {
        printf("\n These pathnames do not occupy any blocks.\n");
        return 1;
    }
    if (correct_count == test_count)
        printf("Block counts for both output files match. checking values...\n");
    for (i = 0; i < correct_count; i++) {
        if (i == 0)
            printf("\n Block numbers start at %s.\n", kc_blocks[i]);
        if (strcmp(kc_blocks[i], test_blocks[i]) != 0) {
            printf("Block mismatch found at %d. kc_blocks[%d] = %s test_blocks[%d] = %s\n", i, i, kc_blocks[i], i, test_blocks[i]);
            return 1;
        }
    }
    printf(" Block numbers end at %s.\n\n", kc_blocks[i-1]);
    printf("Block values matched for each output, success!\n");
    printf("----------------------------------------------\n");
    fclose(test_file);			
    return 0;
}

void main(int argc, char *argv[]) {
    char *test, *correct;
    test = argv[1];
    correct = argv[2];
    compare(correct, test);
    return;
}






