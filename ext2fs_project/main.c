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
    //printf("root refCount = %d\n", root->refCount);

    //printf("creating P0 as running process\n");
    running = &ProcessTable[0];
    running->status = READY;
    running->cwd = root; root->refCount++;
    running->uid = SUPER_USER;
    //printf("root refCount = %d\n", root->refCount);

    // Create Process1 with uid = 1 for non-super user

    //if(test_mode)
        //test();

    while(true)
    {
        char*  input    = NULL;
        char** cmd_argv = NULL;
        int    cmd_argc = 0;
        int  (*cmd_fptr)(int, char**) = NULL; 

        char* dir = find_name(running->cwd);

        do // Prompt user for input
        { 
            if(input_stream == stdin)
                printf("[%d %s]$ ", running->uid, dir); 
        }     
        while(!(input = get_input(input_stream))); // Get user input
        free(dir);

        if(input_stream != stdin)
        {
            putchar('\n');
            puts(input);
        }

        cmd_argv = parse(input, " ");    // Parse input into cmd argv[]
        free(input);

        while(cmd_argv[++cmd_argc]){}    // Determine cmd argc

        cmd_fptr = get_cmd(cmd_argv[0]); // Get the command's function pointer
        cmd_fptr(cmd_argc, cmd_argv);    // Execute the command with parameters
        free_array(cmd_argv);

        if(test_mode)
        {
            char* result = get_input(input_stream);
            puts(result);
            free(result);
        }
    }

    return 0;
}
