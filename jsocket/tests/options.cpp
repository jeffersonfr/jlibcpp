/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jdatagramsocket.h"
#include "jsocketexception.h"
#include "jioexception.h"
#include "jsockettimeoutexception.h"
#include "jsocketlib.h"

#include <iostream>

using namespace jsocket;

int main() {
	InitWindowsSocket();

	char *buffer = new char[1024];
	char *receive = new char[1024];
	int size;

	memset(buffer, 0, 1024);
	memset(receive, 0, 1024);

	try {
		DatagramSocket s("127.0.0.1", 1234);

		SocketOptions *o = s.GetSocketOptions(); 
	
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
	} catch (jio::IOException &e) {
		std::cout << "IO exception" << std::endl;
	} catch (jcommon::Exception &e) {
		std::cout << "Unknown error" << std::endl;
	}

	delete [] buffer;
	delete [] receive;
	
	ReleaseWindowsSocket();

	return 0;
}

