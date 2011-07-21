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
#include "jmemorymap.h"
#include "jmemoryexception.h"

namespace jshared {

MemoryMap::MemoryMap(std::string filename_, jmemory_flags_t flags_, jmemory_permission_t perms_, bool private_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::MemoryMap");

	_permission = perms_;

#ifdef _WIN32
#else
	_is_open = false;
	
	uint32_t f = 0;

	if ((flags_ & JMF_OPEN) != 0) {
		f = (jmemory_flags_t)(f | 0);
	}

	if ((flags_ & JMF_CREAT) != 0) {
		f = (jmemory_flags_t)(f | O_CREAT | O_EXCL | O_TRUNC);
	}

	_fd = open(filename_.c_str(), f | O_RDWR | O_LARGEFILE, (mode_t)0600); 
	
	if (_fd < 0) {
		if ((f | JMF_CREAT) != 0) {
			throw MemoryException("Error creating a shared file");
		} else {
			throw MemoryException("Error opening a shared file");
		}
	}
	
	if ((flags_ & JMF_CREAT) != 0) {
		if (write(_fd, "", 1) < 0) {
			throw MemoryException("Error creating shared memory");
		}
	}

	if (fstat(_fd, &_stats) < 0) {
		throw MemoryException("Error getting stats of shared file");
	}

	uint32_t t = 0;

	if ((perms_ & JMP_EXEC) != 0) {
		t = (jmemory_permission_t)(t | PROT_EXEC);
	}

	if ((perms_ & JMP_READ) != 0) {
		t = (jmemory_permission_t)(t | PROT_READ);
	}

	if ((perms_ & JMP_WRITE) != 0) {
		t = (jmemory_permission_t)(t | PROT_WRITE);
	}

	if ((perms_ & JMP_READ_WRITE) != 0) {
		t = (jmemory_permission_t)(t | PROT_READ | PROT_WRITE);
	}

	if (private_ == true) {
		_start = mmap((caddr_t)0, _stats.st_size, t, MAP_PRIVATE, _fd, 0);
	} else {
		_start = mmap((caddr_t)0, _stats.st_size, t, MAP_SHARED, _fd, 0);
	}

	if (_start == MAP_FAILED) {
		throw MemoryException("Creating memory map failed");
	}
	
	_is_open = true;
#endif
}

MemoryMap::~MemoryMap()
{
	Release();
}

int64_t MemoryMap::Get(char *data_, int64_t size_, int64_t offset_)
{
#ifdef _WIN32
	return 0LL;
#else
	// TODO:: tratar mmap2 com offset
	if (size_ > _stats.st_size) {
		// throw MemoryException("Size cause overflow in memory");
		
		size_ = _stats.st_size;
	}

	memcpy(data_, _start, size_);
	
    return size_;
#endif
}

int64_t MemoryMap::Put(const char *data_, int64_t size_, int64_t offset_)
{
#ifdef _WIN32
	return 0LL;
#else
	// TODO:: tratar mmap2 com offset
	if (size_ > _stats.st_size) {
		throw MemoryException("Size cause overflow in memory");
	}

	memcpy(_start, data_, size_);
	
	return size_;
#endif
}

jmemory_permission_t MemoryMap::GetPermission()
{
	return _permission;
}
	
void MemoryMap::SetPermission(jmemory_permission_t perms_)
{
	_permission = perms_;

#ifdef _WIN32
#else
	unsigned int t = 0;

	if ((_permission & JMP_EXEC) != 0) {
		t = (jmemory_permission_t)(t | PROT_EXEC);
	}

	if ((_permission & JMP_READ) != 0) {
		t = (jmemory_permission_t)(t | PROT_READ);
	}

	if ((_permission & JMP_WRITE) != 0) {
		t = (jmemory_permission_t)(t | PROT_WRITE);
	}

	if ((_permission & JMP_READ_WRITE) != 0) {
		t = (jmemory_permission_t)(t | PROT_READ | PROT_WRITE);
	}

	if ((_permission & JMP_NONE) != 0) {
		t = (jmemory_permission_t)(t | PROT_NONE);
	}

	if (mprotect(_start, _stats.st_size, t) < 0) {
		throw MemoryException("Set permission failed");
	}
#endif
}

int64_t MemoryMap::GetSize()
{
#ifdef _WIN32
	return 0LL;
#else
	return (int64_t)_stats.st_size;
#endif
}

void MemoryMap::Release()
{
	if (_is_open == false) {
		return;
	}
	
#ifdef _WIN32
#else
	int r = munmap(_start, _stats.st_size);
	
	if (r < 0) {
		if (errno == EINVAL) {
			throw MemoryException("We don't like start or length or offset");
		} else if (errno == EAGAIN) {
			throw MemoryException("The file has been locked, or too much memory has been locked");
		} else if (errno == ENOMEM) {
			throw MemoryException("No memory is available, or the process's maximum number of mappings would have been exceeded");
		// } else if (errno == SIGSEGV) {
		// 	throw MemoryException("Attempted write into a region specified to mmap as read-only");
		} else {
			throw MemoryException("Release failed");
		}
	}

	if (close(_fd) < 0) {
	}
#endif
}

}
