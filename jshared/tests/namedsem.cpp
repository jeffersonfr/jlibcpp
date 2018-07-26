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
#include "jshared/jnamedsemaphore.h"

#include <iostream>

#include <string.h>

int main(int argc, char **argv) 
{
	if (argc != 2 || 
			(strcmp(argv[1], "create") != 0 && strcmp(argv[1], "wait") != 0 && strcmp(argv[1], "post") != 0)) {
		std::cout << "use: " << argv[0] << " <create, wait, post>" << std::endl;

		exit(0);
	}

	if (strcmp(argv[1], "create") == 0) {
		jshared::NamedSemaphore s("/tmp/test.sem", 0666);
	} else if (strcmp(argv[1], "post") == 0) {
		jshared::NamedSemaphore s("/tmp/test.sem");
		
		s.Notify();
	} else if (strcmp(argv[1], "wait") == 0) {
		jshared::NamedSemaphore s("/tmp/test.sem");
		
		s.Wait();
	}

	return 0;
}

