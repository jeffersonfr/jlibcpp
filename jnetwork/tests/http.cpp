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
#include "jnetwork/jsocketinputstream.h"
#include "jnetwork/jsocketoutputstream.h"
#include "jnetwork/jsocket.h"
#include "jnetwork/jurl.h"
#include "jnetwork/jnetworklib.h"
#include "jio/jinputstream.h"
#include "jio/joutputstream.h"
#include "jcommon/jstringtokenizer.h"
#include "jexception/jconnectionexception.h"

#include <sstream>
#include <iostream>
#include <stdexcept>

#include <string.h>

void http_stream(jnetwork::URL url) 
{
	std::string buffer = "GET / HTTP/1.0\r\n\r\n";
	std::string receive;

	try {
    jnetwork::Socket c(url.GetHost(), url.GetPort());

    jio::InputStream *i = c.GetInputStream();
    jio::OutputStream *o = c.GetOutputStream();

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
	} catch (jexception::Exception &e) {
		std::cerr << "error: " << e.What() << std::endl;
	}
}

void http_raw(jnetwork::URL url) {
	std::string buffer = "GET / HTTP/1.0\r\n\r\n";

	char receive[4098];
	int length;

	try {
    jnetwork::Socket c(url.GetHost(), url.GetPort());

		c.Send((char *)buffer.c_str(), buffer.size());

		do {
			length = c.Receive(receive, 4096);

			if (length <= 0) {
				std::cerr << "Host disconnect" << std::endl;

				break;
			}

			receive[length] = '\0';

			std::cout << receive << std::flush;
		} while (true);

		c.Close();
	} catch (jexception::Exception &e) {
		std::cerr << "error: " << e.What() << std::endl;
	}
}

int main(int argc, char **argv)
{
	std::string url = "http://127.0.0.1";

	if (argc > 1) {
		url = std::string(argv[1]);
	}

	InitializeSocketLibrary();
		
	http_stream(jnetwork::URL(url));
	// http_raw(jcommon::URL(url));

	ReleaseSocketLibrary();

	return 0;
}

