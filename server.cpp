#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Connection.hpp"

int main(int argc, char** argv)
{
	Connection::clientManager cliMan;
	while(true)
	{
		cliMan.addNewClients();
		cliMan.readFromClients();
	}
}
