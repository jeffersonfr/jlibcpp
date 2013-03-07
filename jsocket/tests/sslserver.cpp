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
#include "jsslserversocket.h"
#include "jsslsocket.h"

#include <iostream>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
using namespace jsocket;

/*
 * Certificate PEM Format (coded)
 *
 * openssl dhparam -check -text -5 1024 -out dh1024.pem
 *
 */
int main(void)
{
	SSLSocket::InitializeSSL();

	SSLServerSocket server(5555);
	SSLSocket *socket;
	
	// Load our certificate
	server.UseCertPassword("certs/cert.pem", "certs/cert_key.pem", "qwerty");
	server.UseDHFile("certs/dh1024.pem");

	while(true){
		socket = (SSLSocket *)server.Accept();
		
		if (socket == NULL) {
			break;
		}

		// Receive message
		char msg[256];
		int r;

		r = socket->Receive(msg, 255);

		if (r > 0) {
			msg[r] = 0;

			std::cout << "Received [" << r << "]: " << msg << std::endl;

			socket->Send("Hello Client!", 13);
		} else {
			std::cout << "Erro no receive" << std::endl;
		}

		socket->Close();

		delete socket;
		socket = NULL;

		break;
	}
	
	SSLSocket::ReleaseSSL();
	
	return 0;
}
