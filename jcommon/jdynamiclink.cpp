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
#include "jcommon/jdynamiclink.h"
#include "jexception/jruntimeexception.h"

#include <dlfcn.h>

namespace jcommon {

DynamicLink::DynamicLink():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::DynamicLink");
}

DynamicLink::~DynamicLink()
{
}

void DynamicLink::Load(std::string lib, jlink_flags_t mode)
{
	int m = 0;

	if (mode == JLF_NOW) {
		m = RTLD_NOW;
	} else if (mode == JLF_LAZY) {
		m = RTLD_LAZY;
	} else if (mode == JLF_LOCAL) {
		m = RTLD_LOCAL;
	} else if (mode == JLF_GLOBAL) {
		m = RTLD_GLOBAL;
	}

	_handle = dlopen(lib.c_str(), m);

	if (_handle == nullptr) {
		throw jexception::RuntimeException(dlerror());
	}
	
	dlerror();
}

void * DynamicLink::FindSymbol(std::string symbol)
{
	return dlsym(_handle, symbol.c_str());
}

void DynamicLink::Unload()
{
	dlclose(_handle);
}

void DynamicLink::ListSymbols(std::string, std::string)
{
}

std::string DynamicLink::GetDependencies()
{
	return "";
}

void DynamicLink::ListLibraries()
{
}

void DynamicLink::Dispatch(std::string f)
{
}

}
