#include "help.h"

void help(void)
{
    //THE FOLLOWING WILL BE THE ALLOCATION AND WRITE TO HELP MENU FOR THE MAIN PAYLOAD
    char help[32][1024];
	int i = 0;	//help indexing the help menu
    strcpy(help[i++], "\n----------HELP MENU----------\n\n");
    strcpy(help[i++], "ls - Print contents of directory\n");
    strcpy(help[i++], "pwd - print working directory\n");
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
    strcpy(help[i++], "clear - clear the current screen\n");
    strcpy(help[i++], "exit - quit from payload\n");
    strcpy(help[i++], "\n------------------------------\n\n");

    strcpy(help[i++], "\n\n\n----------LOCAL NODE HELP MENU----------\n\n");
    strcpy(help[i++], "lpwd - print the local working directory\n");
    strcpy(help[i++], "lls - list the contents of the local working directory\n");
    strcpy(help[i++], "\n------------------------------\n\n");

    //HELP MENU INFO OVER HERE

	for(int j = 0; j < i; j++)
		printf("%s", help[j]);

	return;
}
