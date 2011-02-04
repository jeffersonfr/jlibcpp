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
#include "Stdafx.h"
#include "jmemory.h"

namespace jphysic {

Memory::Memory()
{
	// sysinfo
	
	Builder();
}

Memory::~Memory()
{
}

void Memory::Builder()
{
#ifdef _WIN32
#else
	memset(&info, 0, sizeof(info));
	
	if (sysinfo(&info) < 0) {
		return;
	}
#endif
}

std::string Memory::what()
{
	std::ostringstream o;

#ifdef _WIN32
	return "";
#else
	o << "Uptime: " << info.uptime << std::endl;
	o << "Load average: " << info.loads[2] << std::endl;
	o<< "Total ram: " << info.totalram << std::endl;
	o<< "Free ram: " << info.freeram << std::endl;
	o<< "Shared ram: " << info.sharedram << std::endl;
	o<< "Buffer ram: " << info.bufferram << std::endl;
	o<< "Total swap: " << info.totalswap << std::endl;
	o<< "Free Swap: " << info.freeswap << std::endl;
	o<< "Procs: " << info.procs << std::endl;
	o<< "Total high: " << info.totalhigh << std::endl;
	o<< "Free high: " << info.freehigh << std::endl;
	o<< "Mem unit: " << info.mem_unit << std::endl;
	
	return o.str();
#endif
}

}
