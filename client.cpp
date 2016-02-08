#include "servConnection.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		std::cout << "Usage is 'ip address' 'port number'" << std::endl;
	}
	else
	{
		servConnection::servConnection servConn();
	}
}
