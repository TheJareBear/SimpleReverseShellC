#include "help.h"

void help(void)
{
    //THE FOLLOWING WILL BE THE ALLOCATION AND WRITE TO HELP MENU FOR THE MAIN PAYLOAD
    char help[32][1024];
    strcpy(help[0],  "\n----------HELP MENU----------\n\n");
    strcpy(help[1],  "ls - Print contents of directory\n");
    strcpy(help[2],  "pwd - print working directory\n");
    strcpy(help[3],  "shell - drop into sh shell\n");
    strcpy(help[4],  "shell2 - drop into bash shell\n");
    strcpy(help[5],  "ushell - drop int full bash kind of\n");
    strcpy(help[6],  "zshell - drop into zsh shell kind of\n");
    strcpy(help[7],  "pid - print the working process id\n");
    strcpy(help[8],  "date - get the system datetime\n");
    strcpy(help[9],  "who - print the current users and their ip address\n");
    strcpy(help[10],  "whoami - print the current username\n");
    strcpy(help[11], "passwd - print the passwd file to screen\n");
    strcpy(help[12], "shadow - attempt to print the shadow password to the screen\n");
    strcpy(help[13], "clear - clear the current screen\n");
    strcpy(help[14], "exit - quit from payload\n");
    strcpy(help[15], "\n------------------------------\n\n");
    //HELP MENU INFO OVER HERE

	for(int i = 0; i < 16; i++)
		printf("%s", help[i]);

	return;
}
