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
#include "jsharedmutex.h"
#include "jmemoryexception.h"

#ifdef _WIN32
#else 
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
#endif

namespace jshared {

SharedMutex::SharedMutex(mutex_flags_t flags_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedMutex");
	
#ifdef _WIN32
#else	
	_flags = flags_;
	_memory = NULL;
	_size = 0;
#endif
}

SharedMutex::SharedMutex(void *data_, long long size_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedMutex");
	
#ifdef _WIN32
#else	
	_flags = MCL_CURRENT | MCL_FUTURE;
	_memory = data_;
	_size = size_;

	if ((void *)data_ == NULL) {
		throw MemoryException("Data parameter was NULL");
	}
	
	if (_size <= 0) {
		throw MemoryException("Size parameter was 0");
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
	if ((void *)_memory == NULL) {
		throw MemoryException("Null pointer exception");
	}
	
	if (mlock(_data, _size) < 0) {
		throw MemoryException("Lock memory error");
	}
#endif
}

void SharedMutex::Unlock()
{
#ifdef _WIN32
#else	
	if ((void *)_memory == NULL) {
		throw MemoryException("Null pointer exception");
	}
	
	if (munlock(_data, _size) < 0) {
		throw MemoryException("Unlock memory error");
	}
#endif
}

void SharedMutex::LockAll()
{
#ifdef _WIN32
#else	
	if ((void *)_memory != NULL) {
		throw MemoryException("Null pointer exception");
	}
	
	if (mlockall(_flags) < 0) {
		throw MemoryException("Lock error");
	}
#endif
}

void SharedMutex::UnlockAll()
{
#ifdef _WIN32
#else	
	if ((void *)_memory == NULL) {
		throw MemoryException("Null pointer exception");
	}
	
	if (munlockall() < 0) {
		throw MemoryException("Unlock error");
	}
#endif
}

void SharedMutex::Release()
{
}


}
