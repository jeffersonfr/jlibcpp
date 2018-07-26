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
#include "jshared/jmemorymap.h"

#include <iostream>

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use:: ./ipcmap <id>" << std::endl;

		return -1;
	}

  jshared::MemoryMap *m = NULL;

	std::string key;

	if (argc == 1) {
		key = "server";
	} else if (argc > 1) {
		key = argv[1];
	}

	try {
		m = new jshared::MemoryMap(key, jshared::JMF_CREAT, jshared::JMP_WRITE, true);
	} catch (...) {
		try {
			m = new jshared::MemoryMap(key, jshared::JMF_OPEN, jshared::JMP_WRITE, true);
		} catch (...) {
			std::cout << "Cannot create the memory map" << std::endl;

			return -1;
		}
	}

	sleep(100);

	return 0;
}

