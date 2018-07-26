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
#include "jnetwork/jsslsocket.h"
#include "jnetwork/jsocketlib.h"
#include "jexception/jconnectionexception.h"

#include <iostream>

#include <stdio.h>
#include <unistd.h>

/**
 * \brief SSL Client
 *
 * CRT -> PEM
 *   openssl x509 -in certs/ca.crt -out certs/cacert.pem -outform PEM
 *
 */
int main(void)
{
	InitializeSocketLibrary();

  jnetwork::SSLContext *context = jnetwork::SSLContext::CreateClientContext("certs/cert.pem");
	
	// try {
		jnetwork::SSLSocket mySocket(context, "localhost", 5555);

		/*
		// Check server certificates agains our known trusted certificate
		if (mySocket.UseVerification("certs/cert.pem", NULL) == true) {
			std::cout << "Certificado Verificado com Sucesso !" << std::endl;
		} else {
			std::cout << "Certificado Invalido !" << std::endl;

			mySocket.Close();

			throw jexception::ConnectionException("Peer certificate invalid");
		}
		*/

		// Print server information
		struct jnetwork::peer_cert_info_t info;

		mySocket.GetPeerCertInfo(&info);

		std::cout << "Server certificate information:" << std::endl;
		std::cout << "Signature algorithm: " <<  info.sgnAlgorithm << std::endl;
		std::cout << "Key algorithm: " << info.keyAlgorithm << " (" << info.keySize << " bit)" << std::endl;
		std::cout << "CN   : " << info.commonName << std::endl;
		std::cout << "C    : " << info.countryName << std::endl;
		std::cout << "L    : " << info.localityName << std::endl;
		std::cout << "ST   : " << info.stateOrProvinceName << std::endl;
		std::cout << "O    : " << info.organizationName << std::endl;
		std::cout << "OU   : " << info.organizationalUnitName << std::endl;
		std::cout << "T    : " << info.title << std::endl;
		std::cout << "I    : " << info.initials << std::endl;
		std::cout << "G    : " << info.givenName << std::endl;
		std::cout << "S    : " << info.surname << std::endl;
		std::cout << "D    : " << info.description << std::endl;
		std::cout << "UID  : " << info.uniqueIdentifier << std::endl;
		std::cout << "Email: " << info.emailAddress << std::endl;
		std::cout << "Valid from " << info.notBefore << " to " << info.notAfter << std::endl;
		std::cout << "Serialnumber: " << info.serialNumber << ", version: " << info.version << std::endl;

    std::string pem;
		int r;
		char receive[256];

		mySocket.GetPeerCertPEM(&pem);

		std::cout << "\n" << pem << "\n" << std::endl;

		// Send message to server
		std::cout << "Client running on " << mySocket.GetInetAddress()->GetHostName() << ":" << mySocket.GetPort() << "(" << mySocket.GetInetAddress()->GetHostAddress() << ")" << " sent " << mySocket.Send("Hello Server!", 13) << " bytes" << std::endl;

		if (mySocket.VerifyCertificate() == true) {
			std::cout << "Validation succeded !" << std::endl;
		} else {
			std::cout << "Validation failed !" << std::endl;
		}

		// Receive message from server
		//r = mySocket.Receive(receive, 255);
		r = mySocket.GetInputStream()->Read(receive, 255);

		receive[r] = 0;

		std::cout << "Received: " << receive << std::endl;

		mySocket.Close();
	//} catch (jexception::ConnectionException &e) {
		//std::cout << "Socket Exception: " << e.what() << std::endl;
	//}
	
	delete context;

	ReleaseSocketLibrary();
	
	return 0;
}

