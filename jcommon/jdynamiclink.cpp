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
#include "jcommonlib.h"

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
#ifdef _WIN32
	// _handle = LoadLibrary(_T(lib.c_str()));
#else
	int m = 0;

	if (mode == NOW_LINK) {
		m = RTLD_NOW;
	} else if (mode == LAZY_LINK) {
		m = RTLD_LAZY;
	} else if (mode == LOCAL_LINK) {
		m = RTLD_LOCAL;
	} else if (mode == GLOBAL_LINK) {
		m = RTLD_GLOBAL;
	}

	_handle = dlopen(lib.c_str(), m);

	if (_handle == NULL) {
		throw RuntimeException(dlerror());
	}
#endif
}

void * DynamicLink::FindSymbol(std::string symbol)
{
#ifdef WIN32
	return GetProcAddress((HMODULE)_handle, symbol.c_str());
#else
	void *sym = dlsym(_handle, symbol.c_str());

	if (sym == NULL) {
		return NULL;
	}

	return (void *)sym;
#endif
}

void DynamicLink::Unload()
{
#ifdef _WIN32
	FreeLibrary((HINSTANCE)_handle);
#else
	dlclose(_handle);
#endif
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
