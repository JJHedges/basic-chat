#include <netdb.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/*
 * Class that manages connections to the server by a client.
 */

namespace servConnection
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
			if(status == -1)
			{
				throw(strerror(status));
			}
	
			//Create socket, resulting identifier goes in sockfd
			sockfd = socket(infoResults->ai_family, infoResults->ai_socktype,
							infoResults->ai_protocol);
			if(sockfd == -1)
			{
				throw(strerror(errno));
			}


			//Try to connect the created socket to server, result goes in status
			status = connect(sockfd, infoResults->ai_addr, sizeof(infoResults->ai_addr));
			if(status == -1)
			{
				throw(strerror(errno));
			}
		}
		catch(char *msg)
		{
			std::cerr << msg;
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
				throw(strerror(errno));
			}
		}
		catch(char *msg)
		{
			std::cerr << msg;
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
				throw(strerror(errno));
			}
			else if(result == 1)
			{
				//If server has sent data then read it and output it
				
			}
		}
		catch(char *msg)
		{
			std::cerr << msg;
		}				
	}
}
