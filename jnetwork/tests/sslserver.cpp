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
#include "jnetwork/jsslserversocket.h"
#include "jnetwork/jsslsocket.h"
#include "jnetwork/jnetworklib.h"
#include "jexception/jconnectionexception.h"

#include <iostream>

#include <sys/types.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * DH File 1024
 * openssl dhparam -check -text -5 1024 -out dh1024.pem
 *
 * Certificate Self-Signed (PEM Format)
 * openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout cert_key.pem -out cert.pem
 *
 * Dump Certificate (Text Format)
 * openssl x509 -text -in cert.pem
 *
 */
int main(void)
{
	InitializeSocketLibrary();

  jnetwork::SSLContext *context = jnetwork::SSLContext::CreateServerContext();
	
	// Load our certificate
	if (context->SetCertificate("certs/cert.pem", "certs/cert_key.pem", "qwerty") == false) {
		std::cout << "Load certificates failed ..." << std::endl;

		delete context;

		return 1;
	}

	context->SetDHFile("certs/dh1024.pem");

  jnetwork::SSLServerSocket server(context, 5555);
  jnetwork::SSLSocket *socket = nullptr;
	
	while(true) {
		try {
			std::cout << "Waiting sockets ..." << std::endl;

			socket = (jnetwork::SSLSocket *)server.Accept();

			std::cout << "Certificado Verificado com Sucesso !" << std::endl;

			// Receive message
			char msg[256];
			int r;

			r = socket->Receive(msg, 255);

			if (r > 0) {
				msg[r] = 0;

				std::cout << "Received [" << r << "]: " << msg << std::endl;

				socket->Send("Hello Client!", 13);

				std::cout << "Close socket" << std::endl;

				socket->Close();

				delete socket;
				socket = nullptr;

				if (strncmp(msg, "quit", 4) == 0) {
					break;
				}
			} else {
				std::cout << "Erro no receive" << std::endl;
			}
		} catch (jexception::ConnectionException &e) {
			std::cout << "Socket exception: " << e.What() << std::endl;
		}
	}
	
	delete context;

	std::cout << "Finish" << std::endl;
	
	ReleaseSocketLibrary();
	
	return 0;
}
