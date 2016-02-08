#include <netdb.h>
#include <iostream>
#include <string.h>
#include <errno.h>

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
        	struct addrinfo sockSpecs;
        	struct addrinfo *infoResults;        			 	
    };
    
    servConnection::~servConnection()
    {
        	
    }
        	
    servConnection::servConnection(const char* ipAddr, const char* servPort)
	{
    	memset(&sockSpecs, 0, sizeof(sockSpecs));
    	sockSpecs.ai_family = AF_UNSPEC;
    	sockSpecs.ai_socktype = SOCK_STREAM;
    	sockSpecs.ai_flags = AI_PASSIVE;
    
    	try
		{
			int status = getaddrinfo(ipAddr, servPort, &sockSpecs, &infoResults);
			if(status == -1)
			{
				throw(strerror(status));
			}
		}
		catch (char *msg)
		{
			std::cerr << msg;
		}
	
		try
		{
			sockfd = socket(infoResults->ai_family, infoResults->ai_socktype,
							infoResults->ai_protocol);
			if(sockfd == -1)
			{
				throw(strerror(errno));
			}
		}
		catch (char *msg)
		{
			std::cerr << msg;
		}
		
		try
		{
			int status = connect(sockfd, infoResults->ai_addr, sizeof(infoResults->ai_addr));
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
		
	}
	
	void servConnection::readServer()
	{
		
	}
}
