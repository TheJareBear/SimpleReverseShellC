//this linux handler (also to be renamed)
//should be able to handle any payload (windows, linux, unix, etc)
//connect to open socket and listen for connections, multi threading is way down the line

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

//MY INCLUDES
#include "help.h"

#define DEFPORT 1234            //default port if nothing is given to listen on

int main(int argc, char *argv[])
{
	int server_fd;
	int sock;
	int numread;
	int o = 1;

	int port;

	if(argc >= 2)
		port = atoi(argv[1]);

	else
		port = DEFPORT;

	//creating the structure that handles host info things
	struct sockaddr_in sa;
	int addrlen = sizeof(sa);

	//this will set up the file descriptor for the socket connection
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(!server_fd)
	{
		printf("Something went wrong setting up the socket fd\n");
		return -1;
	}


	//not sure what this is doing... allocating memory or something
	int checkset = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &o, sizeof(o));

	if(checkset)
	{
		printf("Something went wrong preparing the attachment to port\n");
		return -1;
	}


	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(port);

	printf("Set listening port to: %d\n", port);

	//time to bind to port
	int bindVal = bind(server_fd, (struct sockaddr *)&sa, addrlen);

	if(bindVal < 0)
	{
		printf("Bind failed on port: %d\n", port);
		return -1;
	}


	//listen for new connections
	if(listen(server_fd, 3) < 0)
	{
		printf("Failed on listen\n");
		return -1;
	}


	//attempt to accept incoming connection
	if((sock = accept(server_fd, (struct sockaddr *)&sa, (socklen_t *)&addrlen)) < 0)
	{
		printf("Failed to accept incoming connection\n");
		return -1;
	}

	//for debugging
	printf("Connection made! Setting up payload\nOrig: %d\nNow: %d\n", server_fd, sock);


	char buff[8192];
	char command[2048];

	//this is the connection one just to confirm things worked on the client end
	read(sock, buff, sizeof(buff));
	printf("%s", buff);

	int shell = 0;

	while(1)
	{
		for(int i = 0; i < 2048; i++)
		{
			command[i] = 0;
			buff[i] = 0;
		}

		printf("COMMAND> ");

		fgets(command, sizeof(command), stdin);

		if(command[0] == '\n')	//if they just press enter
			continue;

//		command[strlen(command) -1] = '\0'; //dump the newline off of the command handled in payload for now

		if(!strcmp(command, "help\n"))
		{
			help(sock);
			continue;
		}

		if(!strcmp(command, "shell\n"))
			shell++;

		write(sock, command, strlen(command));

		//FOR COMMAND DEBUGGING
		//for command specific debugging printing back command
		//read(sock, buff, sizeof(buff));
		//puts(buff);

		read(sock, buff, sizeof(buff));
		puts(buff);

		if(!strcmp(command, "exit"))
			return 1;

	}


	return 0;
}
