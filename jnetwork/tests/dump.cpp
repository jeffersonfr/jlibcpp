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
#include "jnetwork/jdatagramsocket.h"
#include "jnetwork/jnetworklib.h"

#include <iostream>

#include <string.h>

void dump_stream() 
{
	char *receive = new char[1024];
	int r;

	memset(receive, 0, 1024);

	try {
    jnetwork::DatagramSocket s(4001);

    jio::InputStream *i = s.GetInputStream();

		do {
			r = i->Read(receive, 4096);

			// receive[10] = '\0';

			// std::cout << "char: " << receive << std::endl;
			putchar('+'); fflush(stdout);
		} while (r != 0);

		s.Close();
	} catch (...) {
		std::cout << "error dump_1" << std::endl;
	}
}

void dump_raw() 
{
	char receive[4096];//1024];
	int r;

	try {
    jnetwork::DatagramSocket s(4001);

		jnetwork::SocketOptions *opt = s.GetSocketOptions();

		opt->SetSendTimeout(std::chrono::seconds(2));
		opt->SetReceiveTimeout(std::chrono::seconds(2));
		opt->SetReceiveMaximumBuffer(0x0200000);

		do {
			r = s.Receive(receive, 1500);

			putchar('.'); fflush(stdout);
		} while (r >= 0);

		s.Close();
	} catch (...) {
		std::cout << "error dump_raw" << std::endl;
	}
}

int main()
{
	InitializeSocketLibrary();

	dump_raw();

	ReleaseSocketLibrary();
}

