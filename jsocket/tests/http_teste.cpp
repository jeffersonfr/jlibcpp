#include "jsocketlib.h"
#include "jsocketinputstream.h"
#include "jsocketoutputstream.h"
#include "jsocketexception.h"
#include "jsocket.h"
#include "jinputstream.h"
#include "joutputstream.h"
#include "jstringtokenizer.h"

#include <sstream>
#include <iostream>
#include <stdexcept>

#include <string.h>

using namespace jsocket;
using namespace jio;

void http_stream() {
	std::string buffer = "GET / HTTP/1.0\r\n\r\n";
	std::string receive;
	
	try {
		Socket c("127.0.0.1", 80);

		InputStream *i = c.GetInputStream();
		OutputStream *o = c.GetOutputStream();

		o->Write(buffer.c_str(), buffer.size());
		o->Flush();
	
		char end[4];
		int ch, k = 0;
		
		do {
			ch = i->Read();

			if (ch == '\r' && k != 2) {
				k = 0;
			}

			end[k] = ch;
			k = (k+1)%4;
			
			if (end[0] == '\r' &&
				end[1] == '\n' &&
				end[2] == '\r' &&
				end[3] == '\n') {
				break;
			}

			std::cout << (char)ch;
		} while (ch != 0);
		
		c.Close();
	} catch (SocketException &e) {
		std::cerr << "error: " << e.what() << std::endl;
	}
}

void http_raw() {
	// std::string buffer = "GET / HTTP/1.0\r\n\r\n";
	std::string buffer = ""
											 "GET / HTTP/1.0\r\n"
											 "Accept: */*\r\n"
											 "User-Agent: NSPlayer/4.1.0.3856\r\n"
											 "Host: 187.1.144.78:8080\r\n"
											 "Pragma: xClientGUID={c77e7400-738a-11d2-9add-0020af0a3278}\r\n"
											 "Pragma: no-cache,rate=1.000000,stream-time=0,stream-offset=0:0,request-context=2,max-duration=0\r\n"
											 "Pragma: xPlayStrm=1\r\n"
											 "Pragma: stream-switch-entry=ffff:2:0 ffff:1:0 \r\n"
											 "Pragma: stream-switch-count=2\r\n"
											 "Connection: Close\r\n"
											 "\r\n";
	char receive[4098];
	size_t length;
	
	std::cout << buffer << std::endl;
	try {
		// Socket c("127.0.0.1", 6500);
		Socket c("187.1.144.78", 8080);

		c.Send((char *)buffer.c_str(), buffer.size());
	
		do {
			length = c.Receive(receive, 4096);

			if (length == 0) {
				std::cerr << "Host disconnect" << std::endl;

				break;
			}
			
			// receive[length] = '\0';
			
			// std::cout << receive << std::flush;
			write(1, receive, length);
		} while (true);

		c.Close();
	} catch (SocketException &e) {
		std::cerr << "error: " << e.what() << std::endl;
	}
}

int main()
{
	InitWindowsSocket();
	
	// http_stream();
	http_raw();

	ReleaseWindowsSocket();
}
