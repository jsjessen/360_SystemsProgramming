/* James Jessen
 * 10918967   
 *
 * CptS 360
 * Lab #4
 */

// CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

#include "util.h"
#include "input.h"
#include "parse.h"
#include "cmd.h"

//#define MAX 256

// Define variables
struct hostent *hp;              
struct sockaddr_in  server_addr; 

int sock, r;
int SERVER_IP, SERVER_PORT; 

// clinet initialization code

int client_init(char *argv[])
{
    printf("======= Client Initialization ==========\n");

    printf("1 : get server info\n");
    hp = gethostbyname(argv[1]);
    if (hp==0){
        printf("unknown host %s\n", argv[1]);
        exit(1);
    }

    SERVER_IP   = *(long *)hp->h_addr;
    SERVER_PORT = atoi(argv[2]);

    printf("2 : create a TCP socket\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock<0){
        printf("socket call failed\n");
        exit(2);
    }

    printf("3 : fill server_addr with server's IP and PORT#\n");
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = SERVER_IP;
    server_addr.sin_port = htons(SERVER_PORT);

    // Connect to server
    printf("4 : connecting to server ....\n");
    r = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (r < 0){
        printf("connect failed\n");
        exit(1);
    }

    printf("5 : connected OK to \007\n"); 
    printf("---------------------------------------------------------\n");
    printf("hostname=%s  IP=%s  PORT=%d\n", 
            hp->h_name, inet_ntoa(SERVER_IP), SERVER_PORT);
    printf("---------------------------------------------------------\n");
}

int main(int argc, char *argv[ ])
{
    int n;
    char* line = NULL;
    char* (*cmd_function)(char*); // Pointer to function associated with command
    char** my_argv = NULL;

    if (argc < 3)
    {
        printf("Usage : client ServerName SeverPort\n");
        exit(1);
    }

    client_init(argv);

    while(1)
    {
        char* cmd = NULL;
        char* result = NULL;
        char* filename = NULL;

        free(line);
        free_array(my_argv);

        // Get input from user
        do { printf("Command : "); }
        while((line = get_input()) == NULL);

        // Parse line of input
        my_argv = parse(line, " ");
        cmd = my_argv[0];
        filename = my_argv[1];

        if(strcmp(cmd, "put") == 0) // Put
        {
            write(sock, line, MAX);
            send_file(sock, filename);
        }
        else if(strcmp(cmd, "get") == 0) // Get
        {
            write(sock, line, MAX);
            receive_file(sock, filename); 
        }
        else if(cmd_function = get_cmd(cmd, "client")) // Local Command
        {
            if(result = cmd_function(filename))
            {
                puts(result);
                free(result);
            }
        }
        else // Server Command or Invalid Command 
        {
            write(sock, line, MAX);

            if(result = recieve_string(sock))
            {
                puts(result);
                free(result);
            }
        }
    } //while

    return 0;
}
