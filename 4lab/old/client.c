//********** client.c **********

/* James Jessen             Group #1
 * 10918967                  Seq. #4
 *
 * Austin McShane           Group #2
 * 11350053                  Seq. #29
 *
 * CptS 360
 * Lab #4
 */

#include "shared.h"


// Define variables
struct hostent *hp;
struct sockaddr_in  server_addr;

int sock, r;
int SERVER_IP, SERVER_PORT;

// clinet initialization code

int client_init(char *argv[])
{
	printf("===================== client init =======================\n");

	printf("1 : get server info\n");
	hp = gethostbyname(argv[1]);
	if (hp == 0){
		printf("unknown host %s\n", argv[1]);
		exit(1);
	}

	SERVER_IP = *(long *)hp->h_addr;
	SERVER_PORT = atoi(argv[2]);

	printf("2 : create a TCP socket\n");
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
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

	printf("====================== init done ========================\n");
}


static inline int get_cmdIndex(char* name)
{
	static const char* cmdNames[] = { "lcat",
		"lpwd",
		"lls",
		"lcd",
		"lmkdir",
		"lrmdir",
		"lrm" };
	int index = 0;
	while (cmdNames[index] != NULL)
	{
		if (strcmp(name, cmdNames[index]) == 0)
		{
			return index;
		}
		index++;
	}
	return -1; // unrecognized command

}//getCmdIndex()


main(int argc, char *argv[])
{
	int i, n, index, filesize, count, fd;
	char line[MAX], ans[MAX], sizeStr[MAX], buf[MAX];
	char cmdname[MAX], pathname[MAX];
	char* output;
	char* (*fptr[])(char *) = { (char* (*)())
		my_cat,
		my_pwd,
		my_ls,
		my_cd,
		my_mkdir,
		my_rmdir,
		my_rm };
	if (argc < 3){
		printf("Usage : client ServerName SeverPort\n");
		exit(1);
	}

	client_init(argv);

	while (1)
	{
		printf("\n");
		printf("********************** menu *********************\n");
		printf("* get  put  ls   cd   pwd   mkdir   rmdir   rm  *\n");
		printf("* lcat     lls  lcd  lpwd  lmkdir  lrmdir  lrm  *\n");
		printf("*************************************************\n");

		// Prompt user for input
		do
		{
			printf("Command: ");
			bzero(line, MAX); bzero(cmdname, MAX); bzero(pathname, MAX); // zero out input strings
			fgets(line, MAX, stdin);
			sscanf(line, "%s %s", cmdname, pathname); // parse input, ignoring whitespace
		} while (cmdname[0] == 0);


		if (strcmp(cmdname, "quit") == 0)
		{
			printf("\n");
			exit(EXIT_SUCCESS);
		}
		else if (strcmp(cmdname, "put") == 0)
		{
			write(sock, line, MAX); //send cmd to server

			struct stat sb;

			if (stat(pathname, &sb) != 0)
			{
				write(sock, "BAD", MAX);
				continue;
			}
			sprintf(sizeStr, "SIZE=%d", sb.st_size);
			write(sock, sizeStr, MAX);

			fd = open(pathname, O_RDONLY);
			while (n = read(fd, buf, MAX))
			{
				write(sock, buf, n);
			}

			close(fd);
		}
		else if (strcmp(cmdname, "get") == 0)
		{
			write(sock, line, MAX); //send request

			read(sock, sizeStr, MAX); //receive size/bad
			if (strcmp(sizeStr, "BAD") == 0) { continue; }

			sscanf(sizeStr, "SIZE=%d", &filesize);

			count = 0;
			fd = open(pathname, O_WRONLY | O_CREAT, 0664);
			while (count < filesize)
			{
				n = read(sock, buf, MAX);
				count += n;
				write(fd, buf, n);
			}

			close(fd);
		}
		else
		{
			index = get_cmdIndex(cmdname);
			if (index >= 0)
			{
				output = fptr[index](pathname);
				if (output != NULL)
				{
					printf("%s\n", output);
					free(output);
				}
			}
			else
			{
				// send to server, it might know how to handle it
				write(sock, line, MAX);

				read(sock, sizeStr, MAX); //receive size/bad
				if (strcmp(sizeStr, "BAD") == 0) { continue; }

				sscanf(sizeStr, "SIZE=%d", &filesize);

				count = 0;
				fd = open(pathname, O_WRONLY | O_CREAT, 0664);
				while (count < filesize)
				{
					n = read(sock, buf, MAX - 1);
					buf[MAX - 1] = 0;
					count += n;
					printf("%s", buf);
				}
			}

		}//else not get/put/quit

		printf("\n");

	}//while(1)
}//main()
