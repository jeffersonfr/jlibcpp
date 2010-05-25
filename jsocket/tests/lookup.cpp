#include "jsocketlib.h"

#include <iostream>

using namespace jsocket;

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use:: " << argv[0] << " <host>" << std::endl;

		return -1;
	}

	InitWindowsSocket();

	InetAddress *inet = InetAddress::GetByName(argv[1]);

	std::cout << "by name:: [" << inet->GetHostName() << ", " << inet->GetHostAddress() << "]" << std::endl;

	std::vector<InetAddress *> inets = InetAddress::GetAllByName(argv[1]);

	for (std::vector<InetAddress *>::iterator i=inets.begin(); i!=inets.end(); i++) {
		std::cout << "all by name:: [" << (*i)->GetHostName() << ", " << (*i)->GetHostAddress()%s << "]" << std::endl;
	}

	ReleaseWindowsSocket();
}

