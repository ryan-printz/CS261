#include "TCPConnection.h"
#include <iostream>

int main(void)
{
	std::cout << cb->connectionInfo() << std::endl;

	IConnection * tcp = new TCPConnection("127.0.0.1", gethostbyname(""), 1024);

	std::cout << tcp->connectionInfo() << std::endl;

	IConnection * udp = nullptr;

	std::getchar();
	return 0;
}