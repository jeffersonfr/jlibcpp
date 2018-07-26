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
#include "jshared/jsharedmemory.h"
#include "jexception/jmemoryexception.h"

#include <sys/ipc.h>
#include <sys/shm.h>

#include <string.h>

namespace jshared {

SharedMemory::SharedMemory(key_t key_, int memsize_, int perms_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("SharedMemory");
		
	_memsize = memsize_;
	
	_id = shmget(key_, memsize_, IPC_CREAT | IPC_EXCL | perms_);

	if (_id < 0) {
		/*
		if (errno == EINVAL) {
			throw jexception::MemoryException("Mem size < SHMMIN or > SHMMAXError opening a shared memory");
		} else if (errno == EEXIST) {
			throw jexception::MemoryException("Shared memory key already created");
		} else if (errno == ENOMEM) {
			throw jexception::MemoryException("No memory could be allocated for segment overhead");
		} else {
			throw jexception::MemoryException("Cannot create a new shared memory");
		}
		*/
	
		_id = shmget(key_, _memsize, perms_);

		if (_id < 0) {
			if (errno == ENOSPC) {
				throw jexception::MemoryException("No segment exists for the given key");
			} else if (errno == EACCES) {
				throw jexception::MemoryException("user does not have permission to access the shared memory");
			} else {
				throw jexception::MemoryException("Cannot open a shared memory");
			}
		}
	}
}

SharedMemory::~SharedMemory()
{
	Release();
}

void SharedMemory::Attach()
{
	_shmp = (char *)shmat(_id, 0, 0);

	if (_shmp == (char *)(-1)) {
		if (errno == EACCES) {
			throw jexception::MemoryException("No permission to access the shared memory");
		} else if (errno == ENOMEM) {
			throw jexception::MemoryException("Could not allocate memory");
		} else {
			throw jexception::MemoryException("Attach memory exception");
		}
	}

	memset(_shmp, 0, _memsize);
}

void SharedMemory::Detach()
{
	int r = shmdt(_shmp);

	if (r < 0) {
		if (errno == EACCES) {
			throw jexception::MemoryException("No permission to access the shared memory");
		} else {
			throw jexception::MemoryException("Deatach memory exception");
		}
	}
}

void SharedMemory::Deallocate()
{
	int r = shmctl(_id, IPC_RMID, NULL);

	if (r < 0) {
		if (errno == EACCES) {
			throw jexception::MemoryException("No permission to access the shared memory");
		} else {
			throw jexception::MemoryException("Exception at dealloc shared memory");
		}
	}
}

int SharedMemory::Get(char *data_, int size_)
{
	if ((void *)data_ == NULL) {
		throw jexception::MemoryException("Null pointer in data parameter");
	}
	
	if (size_ <= 0) {
		throw jexception::MemoryException("Invalid value for size parameter");
	}

	memcpy(data_, (char *)_shmp, size_);
	
	return size_;
}

int SharedMemory::Put(const char *data_, int size_)
{
	if ((void *)data_ == NULL) {
		throw jexception::MemoryException("Null pointer in data parameter");
	}
	
	memcpy((char *)_shmp, data_, size_);
	
	return size_;
}

void SharedMemory::Release()
{
	Detach();
	Deallocate();
}

}
