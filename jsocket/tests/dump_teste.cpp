#include "jsocket.h"
#include "jdatagramsocket.h"
#include "jsocketexception.h"
#include "jsocketlib.h"
#include "jsocketinputstream.h"

#include <iostream>

using namespace jsocket;
using namespace jio;

void dump_stream() 
{
	char *receive = new char[1024];
	int r;

	memset(receive, 0, 1024);

	try {
		DatagramSocket s(1234);

		InputStream *i = s.GetInputStream();

		do {
			r = i->Read(receive, 10);

			receive[10] = '\0';

			std::cout << "char: " << receive << std::endl;
		} while (r != 0);

		s.Close();
	} catch (...) {
		std::cout << "error dump_1" << std::endl;
	}
}

void dump_raw() 
{
	char receive[1024];
	int r;

	try {
		DatagramSocket s(1234);

		jsocket::SocketOption *opt = s.GetSocketOption();

		// DO:: opt->SetSendTimeout(time);
		opt->SetReceiveTimeout(6000000);
		opt->SetReceiveMaximumBuffer(0x0200000);

		delete opt;

		do {
			r = s.Receive(receive, 1500);

			std::cout << "UDP Read:: " << r << std::endl;
		} while (r >= 0);

		s.Close();
	} catch (...) {
		std::cout << "error dump_raw" << std::endl;
	}
}

int main()
{
	InitWindowsSocket();

	dump_raw();

	ReleaseWindowsSocket();
}

