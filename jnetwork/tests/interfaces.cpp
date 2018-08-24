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

	std::vector<jnetwork::NetworkInterface *> interfaces = jnetwork::NetworkInterface::GetNetworkInterfaces();

	for (std::vector<jnetwork::NetworkInterface *>::iterator i=interfaces.begin(); i!=interfaces.end(); i++) {
		std::cout << (*i)->What() << std::endl;

		std::vector<jnetwork::NetworkInterface *> subinterfaces = (*i)->GetSubInterfaces();
		
		for (std::vector<jnetwork::NetworkInterface *>::iterator j=subinterfaces.begin(); j!=subinterfaces.end(); j++) {
			std::cout << (*j)->What() << std::endl;
		}
	}

	ReleaseSocketLibrary();
}
