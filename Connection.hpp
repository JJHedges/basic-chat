#include <netdb.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

//Maximum number of connections to be queued up for server during listen()
#define MAXSOCKCONNS 32

namespace Connection
{
	//Class to manage a clients connection to server
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
    
    //Destructor for class, only needs to manage the socket to server
    servConnection::~servConnection()
    {
    	//Shutdown socket to server with no further reading or writing
    	shutdown(sockfd, SHUT_RDWR);
    }
    
    //Class constructor, needs to be provided server ip address and port    	
    servConnection::servConnection(const char* ipAddr, const char* servPort)
	{
		//Clear socket specifications just in case
    	memset(&sockSpecs, 0, sizeof(sockSpecs));
    	//Unspecified address family (ipv4 or ipv6)
    	sockSpecs.ai_family = AF_UNSPEC;
    	//Set socket type to SOCK_STREAM (TCP)
    	sockSpecs.ai_socktype = SOCK_STREAM;
    	
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
			status = connect(sockfd, infoResults->ai_addr, infoResults->ai_addrlen);
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
	
	//Write provided message to server to echo to all clients
	void servConnection::writeServer(char* message)
	{
		int status;
		int size = strlen(message);
		try
		{
			//Writes length of message to server socket
			int tmp = htonl((uint32_t)size);
			status = write(sockfd, &tmp, sizeof(tmp));
			if(status == -1)
			{
				throw(status);
			}
			//Writes message to server socket
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
	
	//Checks for messages then reads from the server socket
	void servConnection::readServer()
	{
		//File descriptor set with just the server socket in it
		fd_set readSet;
		FD_SET(sockfd ,&readSet);
		int result, status;
		//Timeval struct with 0 seconds so select returns instantly
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
			//If server has data for client
			else if(FD_ISSET(sockfd, &readSet))
			{
				uint32_t size;
				char message[256];
				memset(message, '\0', 256);
				//Read length of message from socket
				status = read(sockfd, &size, sizeof(uint32_t));
				if(status == -1)
				{
					throw(status);
				}
				size = ntohl(size);
				//Read message from socket
				status = read(sockfd, message, size);
				if(status == -1)
				{
					throw(status);
				}
				std::cout << message << std::endl;
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}				
	}
	
	//Class to manage clients of the server
	class clientManager
	{
		public:
			clientManager(int);
			~clientManager();
			void addNewClients();
			void readFromClients();
			void echoToClients(char *, int, int);
			
		private:
			//timeval struct with the appropriate timeout value		
			struct timeval timeout;
			//Struct holding servers main socket information		
			struct sockaddr_in server;
			//File descriptor of main server socket and the maximum decriptor seen so far
			int servfd, maxSockfd;
			//File descriptor set holding all current clients
			fd_set currentClients;
	};
	
	//Constructor for client manager, requires port server is to be run on
	clientManager::clientManager(int servPort)
	{
		FD_ZERO(&currentClients);
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(servPort);
		//0 seconds makes select instantly return (poll)
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		try
		{
			int result;
			//Create socket for use over internet, stream type socket with unspecified protocol
			servfd = socket(AF_INET, SOCK_STREAM, 0);
			if(servfd == -1)
			{
				throw(servfd);
			}
			maxSockfd = servfd;
			result = bind(servfd, (struct sockaddr *)&server, sizeof(server));
			if(result == -1)
			{
				throw(errno);
			}
			//Listen for attempted connections to server, queuing up to MAXSOCKCONNS
			result = listen(servfd, MAXSOCKCONNS);
			if(result == -1)
			{
				throw(errno);
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}
		
	}
	
	//Client manager destructor, shutdowns all sockets
	clientManager::~clientManager()
	{
		for(int i = 0; i < maxSockfd + 1; i++)
		{
			if(FD_ISSET(i, &currentClients))
			{
				//Close socket with no more reading
				shutdown(i, SHUT_RDWR);
			}
			FD_ZERO(&currentClients);
		}
		shutdown(servfd, SHUT_RDWR);
	}
	
	//Echoes given message of messageLen to all clients, ignoring the sender
	void clientManager::echoToClients(char* message, int messageLen, int ignore)
	{
		int tmp = htonl(messageLen);
		for(int i = 0; i < maxSockfd + 1; i++)
		{
			//If i is a current client and was not the sender, echo message to them
			if(FD_ISSET(i, &currentClients) && i != ignore)
			{
				//Size of message
				write(i, &tmp, sizeof(uint32_t));
				//Message
				write(i, message, messageLen);
			}
		}
	}
	
	//Reads from clients, if there is a message, echo to all clients
	void clientManager::readFromClients()
	{
		//Temporary set to hold current clients as argument to select is modified in place
		fd_set tmpSet;
		//Copy currentClients into tmpSet
		memcpy(&tmpSet, &currentClients, sizeof(tmpSet));
		//Buffer to be used to check if socket has closed since last check
		char buff[1];		
		try
		{
			//Check all clients for messages
			int result = select(maxSockfd + 1, &tmpSet, NULL, NULL, &timeout);
			if(result == -1)
			{
				throw(errno);
			}
			else if(result != 0)
			{
				for(int i = 0; i < maxSockfd + 1; i++)
				{
					//If i is ready to be read from
					if(FD_ISSET(i, &tmpSet))
					{
						//Check the message isn't blank (indicating shutdown)
						if(recv(i, buff, sizeof(buff), MSG_PEEK | MSG_DONTWAIT) != 0)
						{
							char msg[256];
							int size;
							memset(msg, '\0', 256);
							//Read size of message
							read(i, &size, sizeof(uint32_t));
							size = ntohl(size);
							//Read message
							read(i, &msg, size);
							//Echo to all clients except sender
							echoToClients(msg, size, i);
						}
						//If client has shutdown
						else
						{
							//Remove from currentClients set and shutdown socket
							FD_CLR(i, &currentClients);
							shutdown(i, SHUT_RDWR);
						}
					}
				}
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}
	}
	
	//Add new clients to set as they connect
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
				std::cout << "Adding new connection" << std::endl;
				socklen_t len = sizeof(server);
				int newSockfd = accept(servfd, (struct sockaddr *)&server, &len);
				if(newSockfd == -1)
				{
					throw newSockfd;
				}
				//Character array to hold new clients ip to be output
				char ipStr[INET_ADDRSTRLEN];
				//Fill ipStr with ip address
				inet_ntop(AF_INET, &(server.sin_addr), ipStr, INET_ADDRSTRLEN);
				std::cout << "Connection accepted from: " << ipStr << std::endl;
				//Add new connection to currentClients
				FD_SET(newSockfd, &currentClients);
				//If maxSockfd < newSockfd, update it to newSockfd
				maxSockfd = (maxSockfd < newSockfd) ? newSockfd:maxSockfd;
			}
		}
		catch(int err)
		{
			std::cerr << gai_strerror(err) << std::endl;
		}
	}
}
