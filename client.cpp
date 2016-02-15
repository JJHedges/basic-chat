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
		fd_set cinfd;
		int result;
		struct timeval timeout;
		timeout.tv_usec = 0;
		timeout.tv_sec = 0;
		Connection::servConnection servConn(argv[1], argv[2]);
		while(true)
		{				
			FD_ZERO(&cinfd);
			FD_SET(STDIN_FILENO, &cinfd);
			result = select(STDIN_FILENO + 1, &cinfd, NULL, NULL, &timeout);
			if(result == -1)
			{
				std::cerr << gai_strerror(errno) << std::endl;
				break;
			}
			else if(result != 0)
			{
				memset(message, '\0', 256);
				std::cin >> message;
				if(strcmp(message, "exit") == 0)
				{
					break;
				}
				servConn.writeServer((char*)message);
			}									
			servConn.readServer();
		}
	}
}
