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
#include "jsharedmemory.h"

#include <iostream>

#include <stdlib.h>

using namespace jshared;

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use:: ./ipcmap <id>" << std::endl;

		return -1;
	}

	SharedMemory *m = NULL;

	int key = atoi(argv[1]);

	try {
		m = new SharedMemory(key, 10000, 0644);
	} catch (...) {
		try {
			m = new SharedMemory(key, 10000);
		} catch (...) {
			std::cout << "Cannot create the shared memory" << std::endl;

			return -1;
		}
	}

	sleep(5);

	return 0;
}

