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
#include "jinetaddress4.h"
#include "jsocketlib.h"

#include <iostream>

using namespace jsocket;

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use:: " << argv[0] << " <host>" << std::endl;

		return -1;
	}

	InitWindowsSocket();

	InetAddress *inet = InetAddress4::GetByName(argv[1]);

	std::cout << "by name:: [" << inet->GetHostName() << ", " << inet->GetHostAddress() << "]" << std::endl;

	std::vector<InetAddress *> inets = InetAddress4::GetAllByName(argv[1]);

	for (std::vector<InetAddress *>::iterator i=inets.begin(); i!=inets.end(); i++) {
		std::cout << "all by name:: [" << (*i)->GetHostName() << ", " << (*i)->GetHostAddress() << "]" << std::endl;
	}

	ReleaseWindowsSocket();
}

