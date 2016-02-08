#include "servConnection.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	
	if(argc != 3)
	{
		std::cout << "Usage is 'ip address' 'port number'" << std::endl;
	}
	else
	{
		char *message;
		servConnection::servConnection servConn(argv[1], argv[2]);
		do
		{
			std::cin >> message;
			servConn.writeServer(message);
			servConn.readServer();
			servConn.readServer();
		}while(strcmp(message, "exit") != 0);
	}
}
