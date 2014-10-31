/* James Jessen
 * 10918967   
 *
 * CptS 360
 */

#include <stdio.h>

#include "input.h"

int main(int argc, char* argv[])
{
    init(); // initialize filesystem data structures

    mount_root();

    while(1)
    {
        char* input = NULL;

        int cmd_argc = 0;
        char** cmd_argv = NULL;
        int (*cmd_fptr)(int, char**); 

        do { printf("command : "); }            // Prompt user for input
        while((input = get_input()) == NULL);   // Get user input

        cmd_argv = parse(input, " ");           // Parse input to get cmd argv[]

        while(cmd_argv[cmd_argc + 1])           // Determine cmd argc
        { cmd_argc++; } 

        cmd_fptr = get_cmd(cmd_argv[0]);        // Get the cmd's function pointer
        cmd_fptr(cmd_argc, cmd_argv);           // Execute 

        free(input);
        free_array(cmd_argv);
    }

    return 0;
}
