#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "cmd.h"

#define MAX 10000
typedef struct {
    char *name;
    char *value;
} ENTRY;

ENTRY entry[MAX];

main(int argc, char *argv[]) 
{
    int i, m, r;
    char cwd[128];
    char pathname[256];

    m = getinputs();    // get user inputs name=value into entry[ ]
    getcwd(cwd, 128);   // get CWD pathname

    printf("Content-type: text/html\n\n");
    printf("<p>pid=%d uid=%d cwd=%s\n", getpid(), getuid(), cwd);

    printf("<H1>Echo Your Inputs</H1>");
    printf("You submitted the following name/value pairs:<p>");

    for(i=0; i <= m; i++)
        printf("%s = %s<p>", entry[i].name, entry[i].value);
    printf("<p>");

    /*****************************************************************
      Write YOUR C code here to processs the command
      mkdir dirname
      rmdir dirname
      rm    filename
      cat   filename
      cp    file1 file2
      ls    [dirname] <== ls CWD if no dirname
     *****************************************************************/
    printf("<p>------------------------------------------------</p>");

    // Command   = entry[0]
    // Filename1 = entry[1]
    // Filename2 = entry[2]

    printf("<code>");

    if(entry[1].value)
    {
        if(entry[1].value[0] != '/')
        {
            strcpy(pathname, cwd);
            strcat(pathname, "/");
        }

        strcat(pathname, entry[1].value);
    }

    if(strcmp(entry[0].value, "mkdir") == 0)
    {
        if(my_mkdir(pathname) == 0)
            printf("mkdir - Success</p><p>");
        else
            printf("mkdir - Failure</p><p>");
    }
    else if(strcmp(entry[0].value, "rmdir") == 0)
    {
        if(my_rmdir(pathname) == 0)
            printf("rmdir - Success</p><p>");
        else
            printf("rmdir - Failure</p><p>");

    }
    else if(strcmp(entry[0].value, "rm") == 0)
    {
        if(my_rm(pathname) == 0)
            printf("rm - Success</p><p>");
        else
            printf("rm - Failure</p><p>");
        ;
    }
    else if(strcmp(entry[0].value, "cat") == 0)
    {
        if(my_cat(pathname) == 0)
            printf("cat - Success</p><p>");
        else
            printf("cat - Failure</p><p>");

    }
    else if(strcmp(entry[0].value, "cp") == 0)
    {
        char pathname2[256];

        if(entry[2].value)
        {
            if(entry[2].value[0] != '/')
         {
            strcpy(pathname2, cwd);
            strcat(pathname2, "/");
         }

            strcat(pathname2, entry[2].value);
        }

        if(my_cp(pathname, pathname2) == 0)
            printf("cp - Success</p><p>");
        else
            printf("cp - Failure</p><p>");
    }
    else if(strcmp(entry[0].value, "ls") == 0)
    {
        if(my_ls(pathname) == 0)
            printf("ls - Success</p><p>");
        else
            printf("ls - Failure</p><p>");
    }

    printf("</code>");

    // create a FORM webpage for user to submit again 
    printf("</title>");
    printf("</head>");
    printf("<body bgcolor=\"#0066FF\" link=\"#330033\" leftmargin=8 topmargin=8");
    printf("<p>------------------ DO IT AGAIN ----------------\n");

    printf("<FORM METHOD=\"POST\" ACTION=\"http://cs360.eecs.wsu.edu/~james_jessen/cgi-bin/mycgi\">");

    printf("Enter command : <INPUT NAME=\"command\"> <P>");
    printf("Enter filename1: <INPUT NAME=\"filename1\"> <P>");
    printf("Enter filename2: <INPUT NAME=\"filename2\"> <P>");
    printf("Submit command: <INPUT TYPE=\"submit\" VALUE=\"Click to Submit\"><P>");
    printf("</form>");
    printf("------------------------------------------------<p>");

    printf("</body>");
    printf("</html>");
}
