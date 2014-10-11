//********** server.c **********

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


// Define variables:
struct sockaddr_in  server_addr, client_addr, name_addr;
struct hostent *hp;

int  sock, newsock;                  // socket descriptors
int  serverPort;                     // server port number
int  r, length, n;                   // help variables

// Server initialization code:

int server_init(char *name)
{
	printf("==================== server init ======================\n");
	// get DOT name and IP address of this host

	printf("1 : get and show server host info\n");
	hp = gethostbyname(name);
	if (hp == 0){
		printf("unknown host\n");
		exit(1);
	}
	printf("    hostname=%s  IP=%s\n",
		hp->h_name, inet_ntoa(*(long *)hp->h_addr));

	//  create a TCP socket by socket() syscall
	printf("2 : create a socket\n");
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		printf("socket call failed\n");
		exit(2);
	}

	printf("3 : fill server_addr with host IP and PORT# info\n");
	// initialize the server_addr structure
	server_addr.sin_family = AF_INET;                  // for TCP/IP
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // THIS HOST IP address  
	server_addr.sin_port = 0;   // let kernel assign port

	printf("4 : bind socket to host info\n");
	// bind syscall: bind the socket to server_addr info
	r = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (r < 0){
		printf("bind failed\n");
		exit(3);
	}

	printf("5 : find out Kernel assigned PORT# and show it\n");
	// find out socket port number (assigned by kernel)
	length = sizeof(name_addr);
	r = getsockname(sock, (struct sockaddr *)&name_addr, &length);
	if (r < 0){
		printf("get socketname error\n");
		exit(4);
	}

	// show port number
	serverPort = ntohs(name_addr.sin_port);   // convert to host ushort
	printf("    Port=%d\n", serverPort);

	// listen at port with a max. queue of 5 (waiting clients) 
	printf("5 : server is listening ....\n");
	listen(sock, 5);
	printf("===================== init done =======================\n");
}


static inline int get_cmdIndex(char* name)
{
	static const char* cmdNames[] = { "pwd",
		"ls",
		"cd",
		"mkdir",
		"rmdir",
		"rm" };
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
	int i, n, r, index, count, fd, filesize;
	char *hostname;
	char line[MAX], ans[MAX], buf[MAX], sizeStr[MAX];
	char cmdname[MAX], pathname[MAX];
	char* output;
	char* (*fptr[])(char *) = { (char* (*)())
		my_pwd,
		my_ls,
		my_cd,
		my_mkdir,
		my_rmdir,
		my_rm };

	// Change root to cwd for security
	if (chroot(my_pwd("")) != 0)
	{
		fprintf(stderr, "%s\n", strerror(errno));
	}

	if (argc < 2)
		hostname = "localhost";
	else
		hostname = argv[1];

	server_init(hostname);

	// Try to accept a client request
	while (1)
	{
		printf("server: accepting new connection ....\n");

		// Try to accept a client connection as descriptor newsock
		length = sizeof(client_addr);
		newsock = accept(sock, (struct sockaddr *)&client_addr, &length);
		if (newsock < 0)
		{
			printf("server: accept error\n");
			exit(1);
		}
		printf("server: accepted a client connection from\n");
		printf("-----------------------------------------------\n");
		printf("        IP=%s  port=%d\n", inet_ntoa(client_addr.sin_addr.s_addr),
			ntohs(client_addr.sin_port));
		printf("-----------------------------------------------\n");

		// Processing loop
		while (1)
		{
			bzero(line, MAX); bzero(cmdname, MAX); bzero(pathname, MAX); // zero out input strings
			n = read(newsock, line, MAX);
			if (n == 0)
			{
				printf("server: client died, server loops\n");
				close(newsock);
				break;
			}

			// parse input, ignoring whitespace
			sscanf(line, "%s %s", cmdname, pathname);

			if (strcmp(cmdname, "put") == 0) //
			{
				read(newsock, sizeStr, MAX); //receive size/bad
				if (strcmp(sizeStr, "BAD") == 0) { continue; }

				sscanf(sizeStr, "SIZE=%d", &filesize);

				count = 0;
				fd = open(pathname, O_WRONLY | O_CREAT, 0664);
				while (count < filesize)
				{
					n = read(newsock, buf, MAX);
					count += n;
					write(fd, buf, n);
				}

				close(fd);
			}
			else if (strcmp(cmdname, "get") == 0)
			{
				struct stat sb;

				if (stat(pathname, &sb) != 0)
				{
					write(newsock, "BAD", MAX);
					continue;
				}
				sprintf(sizeStr, "SIZE=%d", sb.st_size);
				write(newsock, sizeStr, MAX);

				fd = open(pathname, O_RDONLY);
				while (n = read(fd, buf, MAX))
				{
					write(newsock, buf, n);
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
						sprintf(sizeStr, "SIZE=%d", strlen(output) + 1);
						write(newsock, sizeStr, MAX);

						write(newsock, output, strlen(output) + 1);


						// printf("%s\n", output); // NEED to send to client for display
						// sendString(newsock, output);
						free(output);
					}
					else
					{
						write(newsock, "BAD", MAX);
					}
				}
				else
				{
					fprintf(stderr, "Error: Command '%s' is not supported.\n", cmdname);
				}
			}

		}//while(1) Processing loop


		// printf("server: read  n=%d bytes; line=[%s]\n", n, line);

		// send the echo line to client 
		//n = write(newsock, line, MAX);

		//printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
		printf("server: ready for next request\n");


	}//while(1) Try to accept a client request
}//main()
