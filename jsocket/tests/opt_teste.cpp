#include "jsocketoption.h"
#include "jsocket.h"
#include "jdatagramsocket.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jsystem.h"
#include "jsocketlib.h"

#include <iostream>

using namespace jsocket;

int main() {
	InitWindowsSocket();

	char *buffer = new char[1024];
	char *receive = new char[1024];
	int size;

	memset(buffer, 0, 1024);
	memset(receive, 1, 1024);

	try {
		DatagramSocket s("127.0.0.1", 1234);

		SocketOption *o = s.GetSocketOption(); 
	
		o->SetReceiveTimeout(10*1000);
		o->SetSendMaximumBuffer(10240);
		o->SetReceiveMaximumBuffer(10240);
	
		delete o;

		std::cout << "Maximun Transfer Unit  :: " << o->GetMaximunTransferUnit() << " bytes" << std::endl;
		std::cout << "Send Maximun Buffer    :: " << o->GetSendMaximumBuffer() << " bytes" << std::endl;
		std::cout << "Receive Maximun Buffer :: " << o->GetReceiveMaximumBuffer() << " bytes" << std::endl;
		std::cout << "Waiting for receive ..." << std::endl;

		size = s.Receive(receive, 1024);

		std::cout << "Receive OK" << std::endl;
		std::cout << "size: " << size << std::endl;

		s.Close();
	} catch (SocketException &e) {
		std::cout << e.what() << std::endl;
	} catch (SocketTimeoutException &e) {
		std::cout << "Socket timeout" << std::endl;
	} catch (jcommon::RuntimeException &e) {
		std::cout << "Unknown error::" << e.what() << " -> " << jcommon::System::GetLastErrorMessage() << std::endl;
	}

	delete [] buffer;
	delete [] receive;
	
	ReleaseWindowsSocket();

	return 0;
}

