/* James Jessen
 * 10918967   
 *
 * CptS 360
 */

#include <stdio.h>

#include <global.h>
#include <input.h>
#include <print.h>
#include <cmd.h>

#include <init.c>
#include <test.c>

int main(int argc, char* argv[])
{
    FILE* input_stream = stdin;
    bool test_mode = false;

    // Verify that user supplied a device name 
    if (argc < 2)
    {
        // Otherwise, show user how to properly execute this program
        printf("\nUsage:\n");
        printf("         my_ext2fs [Device]\n");
        printf("  e.g.   my_ext2fs  myDisk\n\n");

        printf("         my_ext2fs [Device] [InputFile] \n");
        printf("  e.g.   my_ext2fs  myDisk   myInput\n\n");
        exit(EXIT_FAILURE);
    }

    // Check if the user supplied a file to be used as input
    if(argc > 2)
    {
        char* input_path = argv[2];

        // Open the input file in read mode
        if((input_stream = fopen(input_path, "r")) == NULL)
        {
            fprintf(stderr, "Failed to open input file '%s'\n", input_path);
            exit(EXIT_FAILURE);
        }

        // If the file contains the word "test" then assume it is a test file
        if(strstr(input_path, "test") != NULL)
            test_mode = true;
    }

    char* device = argv[1];
    initialize_fs(); 
    mount_root(device);
    initialize_proc();

    if(test_mode)
        test();

    if(input_stream != stdin)
    {
        printf("**************************************\n");
        printf("* Executing Commands from Input File *\n");
        printf("**************************************\n\n");
    }

    while(true)
    {
        char*  input    = NULL;
        char** cmd_argv = NULL;
        int    cmd_argc = 0;
        int  (*cmd_fptr)(int, char**) = NULL; 

        char* cwd = find_name(running->cwd);
        do // Prompt user for input
        { 
            if(input_stream == stdin)
                printf("[%d %s]$ ", running->uid, cwd); 
        }     
        while(!(input = get_input(input_stream))); // Get user input
        free(cwd);

        if(input_stream != stdin)
            printf("%s\n", input);

        // Parse input into cmd argv[]
        cmd_argv = parse(input, " ");    
        free(input);

        // Determine cmd argc
        while(cmd_argv[++cmd_argc]){}   

        // Get the command's function pointer
        cmd_fptr = get_cmd(cmd_argv[0]); 

        // Execute the command
        result_t actual_result = cmd_fptr(cmd_argc, cmd_argv);    
        free_array(cmd_argv);

        // If testing, compare result with what was expected
        if(test_mode)
        {
            char* actual_str   = get_result_str(actual_result);
            char* expected_str = get_input(input_stream);

            if(strcmp(actual_str, expected_str) != 0)
            {
                printf("--------------------------\n");
                printf("EXPECTED : %s\n", expected_str);
                printf("ACTUALLY : %s\n", actual_str);
                printf("==========================\n\n");
            }
            else
                printf("PASS\n\n");

            free(expected_str);
        }

    } // While Loop

    return 0;
}
