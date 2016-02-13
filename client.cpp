#include "Connection.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		std::cout << "Usage is 'ip address' 'port number'" << std::endl;
	}
	else
	{
		char message[256];
		Connection::servConnection servConn(argv[1], argv[2]);
		do
		{
			std::cin >> message;
			servConn.writeServer((char*)message);
			servConn.readServer();
			servConn.readServer();
		}while(strcmp(message, "exit") != 0);
	}
}
