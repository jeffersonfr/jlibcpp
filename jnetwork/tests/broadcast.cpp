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

int main(int argc, char **argv)
{
	if (argc < 3) {
		std::cout << "use:: " << argv[0] << " <host> <port>" << std::endl;

		return -1;
	}

	InitializeSocketLibrary();

	char buf[4096];

  jnetwork::DatagramSocket s(std::string(argv[1]), atoi(argv[2]));

  jnetwork::SocketOptions *opt = s.GetSocketOptions();

  opt->SetBroadcast(true);

  printf("Send:: %d\n", s.Send("jeff", 4));
  printf("Receive:: %d\n", s.Receive(buf, 1500));

  s.Close();

	ReleaseSocketLibrary();
}

