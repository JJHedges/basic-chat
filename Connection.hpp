#include <netdb.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h>

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
    	shutdown(sockfd, 0);
    }
        	
    servConnection::servConnection(const char* ipAddr, const char* servPort)
	{
    	memset(&sockSpecs, 0, sizeof(sockSpecs));
    	sockSpecs.ai_family = AF_UNSPEC;
    	sockSpecs.ai_socktype = SOCK_STREAM;
    	sockSpecs.ai_flags = AI_PASSIVE;
    
    	try
		{
			//Get address info of server on ip ipAddr and port servPort
			int status = getaddrinfo(ipAddr, servPort, &sockSpecs, &infoResults);
			if(status != 0)
			{
				throw(status);
			}
			//Create socket, resulting identifier goes in sockfd
			sockfd = socket(infoResults->ai_family, infoResults->ai_socktype,
							infoResults->ai_protocol);
			if(sockfd == -1)
			{
				throw(sockfd);
			}
			//Try to connect the created socket to server, result goes in status
			status = connect(sockfd, infoResults->ai_addr, sizeof(infoResults->ai_addr));
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
	
	void servConnection::writeServer(char* message)
	{
		int status;
		try
		{
			//Writes to server socket, throws exception on error
			status = write(sockfd, message, 255);
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
		int result, status;
		try
		{
			//Checks if server has sent data
			result = select(sockfd + 1, &readSet, NULL, NULL, NULL);
			if(result == -1)
			{
				throw(result);
			}
			else if(result == 1)
			{
				//If server has sent data then read it and output it
				
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
			
		private:
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
		servfd = socket(AF_INET, SOCK_STREAM, 0);
		maxSockfd = servfd;
		bind(servfd, (struct sockaddr *)&server, sizeof(server));
		listen(servfd, MAXSOCKCONNS);
	}
	
	clientManager::~clientManager()
	{
		
	}
	
	void clientManager::readFromClients()
	{
		
	}
	
	void clientManager::addNewClients()
	{
		//fd_set to contain just the server file descriptor to use in select
		fd_set servSet;
		//timeval struct with the appropriate timeout value
		struct timeval timeout;
		//0 seconds makes select instantly return (poll)
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
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
			else if (result == 1)
			{
				socklen_t len = sizeof(server);
				//T/C FOR ACCEPT
				int newSockfd = accept(servfd, (struct sockaddr *)&server, &len);
				FD_SET(newSockfd, &currentClients);
				maxSockfd = (maxSockfd < newSockfd)?newSockfd:maxSockfd;
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}
	}
}
