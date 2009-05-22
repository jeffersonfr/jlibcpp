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
#include "jmemorymap.h"
#include "jmemoryexception.h"

#ifdef _WIN32
#else 
#include <sys/ioctl.h>
#include <sys/mman.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

namespace jshared {

MemoryMap::MemoryMap(std::string filename_, jmemory_flags_t flags_, jmemory_perms_t perms_, bool private_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::MemoryMap");

#ifdef _WIN32
#else
	_is_open = false;
	
	uint32_t f = 0;

	if ((flags_ & MEM_OPEN) != 0) {
		f = (jmemory_flags_t)(f | 0);
	}

	if ((flags_ & MEM_CREAT) != 0) {
		f = (jmemory_flags_t)(f | O_CREAT | O_EXCL | O_TRUNC);
	}

#ifdef __CYGWIN32__
	_fd = open(filename_.c_str(), t | O_RDWR, (mode_t)0600); 
#else
	_fd = open(filename_.c_str(), f | O_RDWR | O_LARGEFILE, (mode_t)0600); 
#endif
	
	if (_fd < 0) {
		if ((f | MEM_CREAT) != 0) {
			throw MemoryException("Error creating a shared file");
		} else {
			throw MemoryException("Error opening a shared file");
		}
	}
	
	if ((flags_ & MEM_CREAT) != 0) {
		write(_fd, "", 1);
	}

	if (fstat(_fd, &_stats) < 0) {
		throw MemoryException("Error getting stats of shared file");
	}

	uint32_t t = 0;

	if ((perms_ & MEM_EXEC) != 0) {
		t = (jmemory_perms_t)(t | PROT_EXEC);
	}

	if ((perms_ & MEM_READ) != 0) {
		t = (jmemory_perms_t)(t | PROT_READ);
	}

	if ((perms_ & MEM_WRITE) != 0) {
		t = (jmemory_perms_t)(t | PROT_WRITE);
	}

	if ((perms_ & MEM_READ_WRITE) != 0) {
		t = (jmemory_perms_t)(t | PROT_READ | PROT_WRITE);
	}

	if ((perms_ & MEM_NONE) != 0) {
		t = (jmemory_perms_t)(t | PROT_NONE);
	}

	if (private_ == true) {
		_start = mmap((caddr_t)0, _stats.st_size, t, MAP_PRIVATE, _fd, 0);
	} else {
		_start = mmap((caddr_t)0, _stats.st_size, t, MAP_SHARED, _fd, 0);
	}

	if (_start == MAP_FAILED) {
		throw MemoryException("Creating memory map failed");
	}
	
	_perms = perms_;

	_is_open = true;
#endif
}

MemoryMap::~MemoryMap()
{
	Release();
}

long long MemoryMap::Get(char *data_, int size_, int offset_)
{
#ifdef _WIN32
#else
	// TODO:: tratar mmap2 com offset
	if (size_ > _stats.st_size) {
		// throw MemoryException("Size cause overflow in memory");
		//
		size_ = _stats.st_size;
	}

	memcpy(data_, _start, size_);
	
    return size_;
#endif
}

long long MemoryMap::Put(const char *data_, int size_, int offset_)
{
#ifdef _WIN32
#else
	// TODO:: tratar mmap2 com offset
	if (size_ > _stats.st_size) {
		throw MemoryException("Size cause overflow in memory");
	}

	memcpy(_start, data_, size_);
	
    return size_;
#endif
}

void MemoryMap::SetPermission(jmemory_perms_t perms_)
{
#ifdef _WIN32
#else
	unsigned int t = 0;

	if ((_perms & MEM_EXEC) != 0) {
		t = (jmemory_perms_t)(t | PROT_EXEC);
	}

	if ((_perms & MEM_READ) != 0) {
		t = (jmemory_perms_t)(t | PROT_READ);
	}

	if ((_perms & MEM_WRITE) != 0) {
		t = (jmemory_perms_t)(t | PROT_WRITE);
	}

	if ((_perms & MEM_READ_WRITE) != 0) {
		t = (jmemory_perms_t)(t | PROT_READ | PROT_WRITE);
	}

	if ((_perms & MEM_NONE) != 0) {
		t = (jmemory_perms_t)(t | PROT_NONE);
	}

	if (mprotect(_start, _stats.st_size, t) < 0) {
		throw MemoryException("Set permission failed");
	}
#endif
}

long long MemoryMap::GetSize()
{
#ifdef _WIN32
#else
	return (long long)_stats.st_size;
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
