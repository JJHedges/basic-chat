#include "Connection.hpp"
#include <stdlib.h>

int main(int argc, char** argv)
{
	if(argc == 2)
	{
		//Specify port to be run on from arguments
		int port = atoi(argv[1]);
		Connection::clientManager cliMan(port);
		//Loop forever checking for new clients and reading from clients
		while(true)
		{
			cliMan.addNewClients();
			cliMan.readFromClients();
		}
	}
	else
	{
		std::cout << "Usage is ./server 'port'" << std::endl;
	}
}
