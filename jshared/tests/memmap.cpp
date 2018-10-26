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
#include "jio/jfile.h"
#include "jexception/jexception.h"

#include <iostream>
#include <thread>

#include <string.h>

const std::string MEM_FILE("/tmp/mem.ui");

int main(int argc, char **argv)
{
	try {
    jio::File 
      *file = jio::File::CreateFile(MEM_FILE);

    if (file == nullptr) {
      std::cout << "File [" << MEM_FILE << "] already created !" << std::endl;
    }

    file = jio::File::OpenFile(MEM_FILE, (jio::jfile_flags_t)(jio::JFF_READ_WRITE | jio::JFF_LARGEFILE));

    if (file == nullptr) {
      std::cout << "Unable to open the file [" << MEM_FILE << "]" << std::endl;

      return -1;
    }

    jshared::MemoryMap 
      mem(file, 1024, true);

    uint8_t *address = mem.GetAddress();

    memset(address, '1', 10);
	} catch (jexception::Exception &e) {
		std::cout << e.GetMessage() << std::endl;

    return -1;
	}

	return 0;
}

