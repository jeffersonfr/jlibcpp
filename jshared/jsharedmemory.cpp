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
#include "jsharedmemory.h"
#include "jmemoryexception.h"

namespace jshared {

#ifdef _WIN32
SharedMemory::SharedMemory(int key_, int memsize_, int perms_):
#else 
SharedMemory::SharedMemory(key_t key_, int memsize_, int perms_):
#endif
	jcommon::Object()
{
	jcommon::Object::SetClassName("SharedMemory");
		
#ifdef _WIN32
#else	
	_memsize = memsize_;
	
	_id = shmget(key_, memsize_, IPC_CREAT | IPC_EXCL | perms_);

	if (_id < 0) {
		/*
		if (errno == EINVAL) {
			throw SharedMemoryException("Mem size < SHMMIN or > SHMMAXError opening a shared memory");
		} else if (errno == EEXIST) {
			throw SharedMemoryException("Shared memory key already created");
		} else if (errno == ENOMEM) {
			throw SharedMemoryException("No memory could be allocated for segment overhead");
		} else {
			throw SharedMemoryException("Cannot create a new shared memory");
		}
		*/
	
		_id = shmget(key_, _memsize, perms_);

		if (_id < 0) {
			if (errno == ENOSPC) {
				throw SharedMemoryException("No segment exists for the given key");
			} else if (errno == EACCES) {
				throw SharedMemoryException("user does not have permission to access the shared memory");
			} else {
				throw SharedMemoryException("Cannot open a shared memory");
			}
		}
	}
#endif
}

SharedMemory::~SharedMemory()
{
	Release();
}

void SharedMemory::Attach()
{
#ifdef _WIN32
#else	
	_shmp = (char *)shmat(_id, 0, 0);

	if (_shmp == (char *)(-1)) {
		if (errno == EACCES) {
			throw SharedMemoryException("No permission to access the shared memory");
		} else if (errno == ENOMEM) {
			throw SharedMemoryException("Could not allocate memory");
		} else {
			throw SharedMemoryException("Attach memory exception");
		}
	}

	memset(_shmp, 0, _memsize);
#endif
}

void SharedMemory::Detach()
{
#ifdef _WIN32
#else	
	int r = shmdt(_shmp);

	if (r < 0) {
		if (errno == EACCES) {
			throw SharedMemoryException("No permission to access the shared memory");
		} else {
			throw SharedMemoryException("Deatach memory exception");
		}
	}
#endif
}

void SharedMemory::Deallocate()
{
#ifdef _WIN32
#else	
	int r = shmctl(_id, IPC_RMID, NULL);

	if (r < 0) {
		if (errno == EACCES) {
			throw SharedMemoryException("No permission to access the shared memory");
		} else {
			throw SharedMemoryException("Exception at dealloc shared memory");
		}
	}
#endif
}

int SharedMemory::Get(char *data_, int size_)
{
#ifdef _WIN32
	return 0;
#else	
	if ((void *)data_ == NULL) {
		throw SharedMemoryException("Null pointer in data parameter");
	}
	
	if (size_ <= 0) {
		throw SharedMemoryException("Invalid value for size parameter");
	}

	memcpy(data_, (char *)_shmp, size_);
	
	return size_;
#endif
}

int SharedMemory::Put(const char *data_, int size_)
{
#ifdef _WIN32
	return 0;
#else	
	if ((void *)data_ == NULL) {
		throw SharedMemoryException("Null pointer in data parameter");
	}
	
	memcpy((char *)_shmp, data_, size_);
	
	return size_;
#endif
}

void SharedMemory::Release()
{
#ifdef _WIN32
#else	
	Detach();
	Deallocate();
#endif
}

}
