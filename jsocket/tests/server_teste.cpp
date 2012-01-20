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
#include "jserversocket.h"
#include "jsocket.h"
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
