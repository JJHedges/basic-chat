#include <netdb.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define MAXSOCKCONNS 32

/*
 * Class that manages connections to the server by a client.
 */

namespace Connection
{
    class servConnection
    {
        public:
        	servConnection(const char*, const char*);
        	~servConnection();
            void readServer();
        	void writeServer(char*);
        	
            
        private:
        	int sockfd;
        	//Specifications for the type of socket that we want to set up
        	struct addrinfo sockSpecs;
        	//Results of getaddrinfo go here once called
        	struct addrinfo *infoResults;        			 	
    };
    
    servConnection::~servConnection()
    {
    	shutdown(sockfd, SHUT_RDWR);
    }
        	
    servConnection::servConnection(const char* ipAddr, const char* servPort)
	{
    	memset(&sockSpecs, 0, sizeof(sockSpecs));
    	sockSpecs.ai_family = AF_UNSPEC;
    	sockSpecs.ai_socktype = SOCK_STREAM;
    	
    	try
		{
			//Get address info of server on ip ipAddr and port servPort
			int status = getaddrinfo(ipAddr, servPort, &sockSpecs, &infoResults);
			if(status != 0)
			{
				//std::cerr << "here";
				throw(status);
			}
			//Create socket, resulting identifier goes in sockfd
			sockfd = socket(infoResults->ai_family, infoResults->ai_socktype,
							infoResults->ai_protocol);
			if(sockfd == -1)
			{
				//std::cerr << "here";
				throw(sockfd);
			}
			//Try to connect the created socket to server, result goes in status
			status = connect(sockfd, infoResults->ai_addr, infoResults->ai_addrlen);
			if(status == -1)
			{
				//std::cerr << "here";
				throw(status);
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}	
	}
	
	void servConnection::writeServer(char* message)
	{
		int status;
		int size = strlen(message);
		try
		{
			//Writes to server socket, throws exception on error
			int tmp = htonl((uint32_t)size);
			status = write(sockfd, &tmp, sizeof(tmp));
			if(status == -1)
			{
				throw(status);
			}
			status = write(sockfd, message, size);
			if(status == -1)
			{
				throw(status);
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}
	}
	
	void servConnection::readServer()
	{
		fd_set readSet;
		FD_SET(sockfd ,&readSet);
		int result, status;
		struct timeval timeout;
		timeout.tv_usec = 0;
		timeout.tv_sec = 0;
		try
		{
			//Checks if server has sent data
			result = select(sockfd + 1, &readSet, NULL, NULL, &timeout);
			if(result == -1)
			{
				throw(result);
			}
			else if(FD_ISSET(sockfd, &readSet))
			{
				//If server has sent data then read it and output it
				uint32_t size;
				char message[256];
				memset(message, '\0', 256);
				status = read(sockfd, &size, sizeof(uint32_t));
				if(status == -1)
				{
					throw(status);
				}
				size = ntohl(size);
				status = read(sockfd, message, size);
				if(status == -1)
				{
					throw(status);
				}
				std::cout << "External message: " << message << std::endl;
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}				
	}
	
	class clientManager
	{
		public:
			clientManager();
			~clientManager();
			void addNewClients();
			void readFromClients();
			void echoToClients(char *, int);
			
		private:
			//timeval struct with the appropriate timeout value		
			struct timeval timeout;		
			struct sockaddr_in server;
			int servfd, maxSockfd;
			fd_set currentClients;
	};
	
	clientManager::clientManager()
	{
		//ADD TRY CATCH ON SOCKET/BIND/LISTEN
		FD_ZERO(&currentClients);
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(12345);
		//0 seconds makes select instantly return (poll)
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		servfd = socket(AF_INET, SOCK_STREAM, 0);
		maxSockfd = servfd;
		bind(servfd, (struct sockaddr *)&server, sizeof(server));
		listen(servfd, MAXSOCKCONNS);
	}
	
	clientManager::~clientManager()
	{
		for(int i = 0; i < maxSockfd + 1; i++)
		{
			if(FD_ISSET(i, &currentClients))
			{
				//Close socket with no more reading
				shutdown(i, SHUT_RD);
			}
			FD_ZERO(&currentClients);
		}
		shutdown(servfd, SHUT_RD);
	}
	
	void clientManager::echoToClients(char* message, int messageLen)
	{
		int tmp = htonl(messageLen);
		for(int i = 0; i < maxSockfd + 1; i++)
		{
			if(FD_ISSET(i, &currentClients))
			{
				std::cout << "i: " << i << " messageLen: " << messageLen << std::endl;
				std::cout << "tmp: " << tmp << " message: " << message << std::endl;
				write(i, &tmp, sizeof(uint32_t));
				int writeCount = write(i, message, messageLen);
				std::cout << writeCount << " " << sizeof(char) << std::endl;
			}
		}
	}
	
	void clientManager::readFromClients()
	{
		//Temporary set to hold current clients as argument to select is modified in place
		fd_set tmpSet;
		memcpy(&tmpSet, &currentClients, sizeof(tmpSet));		
		try
		{
			int result = select(maxSockfd + 1, &tmpSet, NULL, NULL, &timeout);
			//std::cout << result << std::endl;
			if(result == -1)
			{
				throw(errno);
			}
			else if(result != 0)
			{
				for(int i = 0; i < maxSockfd + 1; i++)
				{
					if(FD_ISSET(i, &tmpSet))
					{
						char msg[256];
						int size;
						memset(msg, '\0', 256);
						read(i, &size, sizeof(uint32_t));
						size = ntohl(size);
						std::cout << "Size: " << size << std::endl;
						read(i, &msg, size);
						std::cout << "Message: " << msg << std::endl;
						echoToClients(msg, size);
					}
				}
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}
	}
	
	void clientManager::addNewClients()
	{
		//fd_set to contain just the server file descriptor to use in select
		fd_set servSet;		
		//Zero the fd_set to make sure memory is clear
		FD_ZERO(&servSet);
		//Add servfd to fd_set
		FD_SET(servfd, &servSet);
		try
		{
			int result = select(servfd + 1, &servSet, NULL, NULL, &timeout);
			if(result == -1)
			{
				throw(errno);
			}
			else if(result == 1)
			{
				std::cout << "Adding new conn" << std::endl;
				socklen_t len = sizeof(server);
				//T/C FOR ACCEPT
				int newSockfd = accept(servfd, (struct sockaddr *)&server, &len);
				std::cout << servfd << " " << newSockfd << std::endl;
				FD_SET(newSockfd, &currentClients);
				maxSockfd = (maxSockfd < newSockfd) ? newSockfd:maxSockfd;
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}
	}
}
