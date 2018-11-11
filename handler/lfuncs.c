#include "lfuncs.h"

//this header file will contain a set of functions to run against the local host (like lls or lpwd in meterpreter)
void lls(void) //local ls
{
    system("/bin/ls");
}

void lpwd(void) //local pwd
{
	system("pwd");
}


