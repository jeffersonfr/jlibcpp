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

int main(void)
{
	SSLServerSocket listener(5555);
	SSLSocket *s;
	
	// Load our certificate
	listener.UseCertPassword("cert.pem", "cert_key.pem", "qwerty");
	listener.UseDHFile("dh1024.pem");

	while(true){
		s = (SSLSocket *)listener.Accept();
		
		// Receive message
		char msg[256];
		int r;

		r = s->Receive(msg, 255);

		if (r > 0) {
			msg[r] = 0;

			std::cout << "Received [" << r << "]: " << msg << std::endl;

			s->Send("Hello Client!", 13);
		} else {
			std::cout << "Erro no receive" << std::endl;
		}

		s->Close();

		delete s;

		return 0;

		delete s;
		s = NULL;
	}
	
	return 0;
}
