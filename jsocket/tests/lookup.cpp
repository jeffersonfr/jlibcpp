#include "jsocket.h"
#include "jdatagramsocket.h"
#include "jsocketexception.h"
#include "jsocketlib.h"
#include "jsocketinputstream.h"

#include <iostream>

using namespace jsocket;

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("use:: %s <host>\n", argv[0]);

		return -1;
	}

	InitWindowsSocket();

	InetAddress *inet = InetAddress::GetByName(argv[1]);

	printf("by name:: [%s, %s]\n", inet->GetHostName().c_str(), inet->GetHostAddress().c_str());

	std::vector<InetAddress *> inets = InetAddress::GetAllByName(argv[1]);

	for (std::vector<InetAddress *>::iterator i=inets.begin(); i!=inets.end(); i++) {
		printf("all by name:: [%s, %s]\n", (*i)->GetHostName().c_str(), (*i)->GetHostAddress().c_str());
	}

	ReleaseWindowsSocket();
}

