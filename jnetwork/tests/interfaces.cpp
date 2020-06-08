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
#include "jnetwork/jnetworkinterface.h"
#include "jnetwork/jnetworklib.h"

#include <iostream>

int main(int argc, char **argv)
{
	InitializeSocketLibrary();

	std::vector<jnetwork::jnetworkinterface_info_t> 
    ifs = jnetwork::NetworkInterface::GetInterfaces();

	for (auto &ifa : ifs) {
		std::cout << ifa.name << std::endl;

    for (auto &addr : ifa.addresses) {
      std::cout << "\taddr: " << addr.addr << std::endl;
      std::cout << "\tmask: " << addr.mask << std::endl;
      std::cout << "\tbroadaddr: " << addr.broadaddr << std::endl;
    }
	}

	ReleaseSocketLibrary();
}

