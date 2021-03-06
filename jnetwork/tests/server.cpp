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
#include "jnetwork/jserversocket.h"
#include "jnetwork/jsocket.h"
#include "jnetwork/jnetworklib.h"
#include "jexception/jexception.h"

#include <iostream>

#include <string.h>

int main(int argc, char **argv) 
{
  if (argc < 2) {
    std::cout << argv[0] << " <port>" << std::endl;

    return -1;
  }

	InitializeSocketLibrary();

	char *receive = new char[1024+1];
	
	try {
    jnetwork::ServerSocket server(atoi(argv[1]), 1);

    jnetwork::Socket *client;

		client = server.Accept();
	
		if (client != nullptr) {
			int r;
			
			do {
				r = client->GetInputStream()->Read(receive, 1024); 
	
				receive[r] = '\0';

				if (strncmp(receive, "\n\n", 4) == 0) {
					break;
				}
			
				std::cout << receive << std::flush;
			} while (r > 0);
		}
		
		client->Close();
	} catch (jexception::Exception &e) {
    e.PrintStackTrace();
	}

	ReleaseSocketLibrary();

	return 0;
}
