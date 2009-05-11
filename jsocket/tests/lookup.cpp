#include "jsocket.h"
#include "jdatagramsocket.h"
#include "jsocketexception.h"
#include "jsocketlib.h"
#include "jsocketinputstream.h"

#include <iostream>

using namespace jsocket;

int main()
{
	InitWindowsSocket();

	InetAddress *inet = InetAddress::GetByName("google.com");

	printf("by name:: [%s, %s]\n", inet->GetHostName().c_str(), inet->GetHostAddress().c_str());

	std::vector<InetAddress *> inets = InetAddress::GetAllByName("google.com");

	for (std::vector<InetAddress *>::iterator i=inets.begin(); i!=inets.end(); i++) {
		printf("all by name:: [%s, %s]\n", (*i)->GetHostName().c_str(), (*i)->GetHostAddress().c_str());
	}

	ReleaseWindowsSocket();
}

