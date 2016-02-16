#include "Connection.hpp"

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		std::cout << "Usage is ./client 'ip address' 'port number'" << std::endl;
	}
	else
	{
		//Array to receive messages
		char message[256];
		//File descriptor set to hold cin as a file descriptor
		fd_set cinfd;
		int result;
		//Timeout struct, set to 0 in both fields so select will return instantly
		struct timeval timeout;
		timeout.tv_usec = 0;
		timeout.tv_sec = 0;
		Connection::servConnection servConn(argv[1], argv[2]);
		std::cout << "Connected to server at ip: " << argv[1] << " and port: " 
					<< argv[2] << std::endl;
		while(true)
		{	
			//Clear file descriptor set and add stdin file descriptor			
			FD_ZERO(&cinfd);
			FD_SET(STDIN_FILENO, &cinfd);
			//Select with stdin and instant return for non-blocking input
			result = select(STDIN_FILENO + 1, &cinfd, NULL, NULL, &timeout);
			if(result == -1)
			{
				std::cerr << gai_strerror(errno) << std::endl;
				break;
			}
			else if(result != 0)
			{
				//Clear message array
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
