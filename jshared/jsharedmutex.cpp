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
#include "jexception/jnullpointerexception.h"

#include <sys/mman.h>

namespace jshared {

SharedMutex::SharedMutex(MemoryMap *mem):
	jcommon::Object()
{
	if (mem == nullptr) {
		throw jexception::NullPointerException("MemoryMap must be valid");
	}
	
	jcommon::Object::SetClassName("jshared::SharedMutex");
	
	_mmap = mem;
}

SharedMutex::~SharedMutex()
{
}

void SharedMutex::Lock()
{
	if (mlock(_mmap->GetAddress(), (size_t)_mmap->GetLength()) < 0) {
		throw jexception::MemoryException("Lock memory error");
	}
}

void SharedMutex::Unlock()
{
	if (munlock(_mmap->GetAddress(), (size_t)_mmap->GetLength()) < 0) {
		throw jexception::MemoryException("Unlock memory error");
	}
}

void SharedMutex::LockAll()
{
	if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
		throw jexception::MemoryException("Lock error");
	}
}

void SharedMutex::UnlockAll()
{
	if (munlockall() < 0) {
		throw jexception::MemoryException("Unlock error");
	}
}

}
