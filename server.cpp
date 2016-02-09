#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXSOCKCONNS 32

void addNewClients()
{
	select();
}

int main(int argc, char** argv)
{
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(12345);
	int sockfd = (AF_INET, SOCK_STREAM, 0);
	bind(sockfd, (struct sockaddr *)&server, sizeof(server));
	listen(sockfd, MAXSOCKCONNS);
	while(true)
	{
		
	}
}
