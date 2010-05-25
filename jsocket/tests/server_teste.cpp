#include "jsocketlib.h"

#include <iostream>
#include <stdexcept>

using namespace jsocket;

int main() {
	InitWindowsSocket();

	char *receive = new char[100+1];
	
	try {
		ServerSocket server(3200, 1);

		Socket *client;

		client = server.Accept();
	
		if (client != NULL) {
			int r;
			
			do {
				r = client->GetInputStream()->Read(receive, 100); 
	
				receive[r-1] = '\0';

				if (strncmp(receive, "quit", 4) == 0) {
					break;
				}
			
				std::cout << receive << std::endl;
			} while (r > 0);
		}
		
		client->Close();
	} catch (std::runtime_error &e) {
		std::cout << "error " << e.what() << std::endl;
	}

	ReleaseWindowsSocket();

	return 0;
}
