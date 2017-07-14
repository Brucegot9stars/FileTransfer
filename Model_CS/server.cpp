#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdio>
#include <cstring>

#define MAXBUFFERSIZE 1024
#define MAXFILENAMESIZE 512

int main(int argc, char* argv[])
{

	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(server_addr));

	server_addr.sin_family = AF_INET;




}
