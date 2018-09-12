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
#include "jshared/jsharedmutex.h"
#include "jexception/jmemoryexception.h"

#include <sys/mman.h>

namespace jshared {

SharedMutex::SharedMutex(jsharedmutex_flags_t flags_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedMutex");
	
	_flags = flags_;
	_shmp = nullptr;
	_size = 0LL;
}

SharedMutex::SharedMutex(void *data_, int64_t size_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedMutex");
	
	_flags = (jsharedmutex_flags_t)(JSF_CURRENT | JSF_FUTURE);
	_shmp = (char *)data_;
	_size = size_;

	if ((void *)data_ == nullptr) {
		throw jexception::MemoryException("Data parameter was nullptr");
	}
	
	if (_size <= 0LL) {
		throw jexception::MemoryException("Size parameter was 0");
	}
}

SharedMutex::~SharedMutex()
{
	Release();
}

void SharedMutex::Lock()
{
	if ((void *)_shmp == nullptr) {
		throw jexception::MemoryException("Null pointer exception");
	}
	
	if (mlock(_shmp, (size_t)_size) < 0) {
		throw jexception::MemoryException("Lock memory error");
	}
}

void SharedMutex::Unlock()
{
	if ((void *)_shmp == nullptr) {
		throw jexception::MemoryException("Null pointer exception");
	}
	
	if (munlock(_shmp, (size_t)_size) < 0) {
		throw jexception::MemoryException("Unlock memory error");
	}
}

void SharedMutex::LockAll()
{
	if ((void *)_shmp != nullptr) {
		throw jexception::MemoryException("Null pointer exception");
	}
	
	int flags = 0;

	if ((_flags & JSF_CURRENT) != 0) {
		flags = flags | MCL_CURRENT;
	}

	if ((_flags & JSF_FUTURE) != 0) {
		flags = flags | MCL_FUTURE;
	}

	if (mlockall(flags) < 0) {
		throw jexception::MemoryException("Lock error");
	}
}

void SharedMutex::UnlockAll()
{
	if ((void *)_shmp == nullptr) {
		throw jexception::MemoryException("Null pointer exception");
	}
	
	if (munlockall() < 0) {
		throw jexception::MemoryException("Unlock error");
	}
}

void SharedMutex::Release()
{
}

}
