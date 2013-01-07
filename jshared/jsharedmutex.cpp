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
#include "jsharedmutex.h"
#include "jmemoryexception.h"

namespace jshared {

SharedMutex::SharedMutex(jsharedmutex_flags_t flags_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedMutex");
	
#ifdef _WIN32
#else	
	_flags = flags_;
	_shmp = NULL;
	_size = 0LL;
#endif
}

SharedMutex::SharedMutex(void *data_, int64_t size_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedMutex");
	
#ifdef _WIN32
#else	
	_flags = (jsharedmutex_flags_t)(JSF_CURRENT | JSF_FUTURE);
	_shmp = (char *)data_;
	_size = size_;

	if ((void *)data_ == NULL) {
		throw SharedMemoryException("Data parameter was NULL");
	}
	
	if (_size <= 0LL) {
		throw SharedMemoryException("Size parameter was 0");
	}
#endif
}

SharedMutex::~SharedMutex()
{
	Release();
}

void SharedMutex::Lock()
{
#ifdef _WIN32
#else	
	if ((void *)_shmp == NULL) {
		throw SharedMemoryException("Null pointer exception");
	}
	
	if (mlock(_shmp, (size_t)_size) < 0) {
		throw SharedMemoryException("Lock memory error");
	}
#endif
}

void SharedMutex::Unlock()
{
#ifdef _WIN32
#else	
	if ((void *)_shmp == NULL) {
		throw SharedMemoryException("Null pointer exception");
	}
	
	if (munlock(_shmp, (size_t)_size) < 0) {
		throw SharedMemoryException("Unlock memory error");
	}
#endif
}

void SharedMutex::LockAll()
{
#ifdef _WIN32
#else	
	if ((void *)_shmp != NULL) {
		throw SharedMemoryException("Null pointer exception");
	}
	
	int flags = 0;

	if ((_flags & JSF_CURRENT) != 0) {
		flags = flags | MCL_CURRENT;
	}

	if ((_flags & JSF_FUTURE) != 0) {
		flags = flags | MCL_FUTURE;
	}

	if (mlockall(flags) < 0) {
		throw SharedMemoryException("Lock error");
	}
#endif
}

void SharedMutex::UnlockAll()
{
#ifdef _WIN32
#else	
	if ((void *)_shmp == NULL) {
		throw SharedMemoryException("Null pointer exception");
	}
	
	if (munlockall() < 0) {
		throw SharedMemoryException("Unlock error");
	}
#endif
}

void SharedMutex::Release()
{
}


}
