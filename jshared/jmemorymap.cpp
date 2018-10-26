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
#include "jshared/jmemorymap.h"
#include "jio/jfile.h"
#include "jexception/jmemoryexception.h"
#include "jexception/jnullpointerexception.h"

#include <sys/mman.h>

#include <fcntl.h>
#include <string.h>

namespace jshared {

MemoryMap::MemoryMap(jio::File *file, int length, bool shared, jio::jfile_permissions_t perms):
	jcommon::Object()
{
  if (file == nullptr) {
		throw jexception::NullPointerException("File pointer must be valid");
  }

	jcommon::Object::SetClassName("jshared::MemoryMap");

  _file = file;
	_length = length;

	int flags = PROT_NONE;

	if ((perms & jio::JFP_USR_READ) != 0) {
		flags = flags | PROT_READ;
	}

	if ((perms & jio::JFP_USR_WRITE) != 0) {
		flags = flags | PROT_WRITE;
	}

	if ((perms & jio::JFP_USR_EXEC) != 0) {
		flags = flags | PROT_EXEC;
	}

	if (shared == false) {
		_address = (uint8_t *)mmap(nullptr, length, flags, MAP_PRIVATE, _file->GetDescriptor(), 0);
	} else {
		_address = (uint8_t *)mmap(nullptr, length, flags, MAP_SHARED, _file->GetDescriptor(), 0);
	}

	if (_address == MAP_FAILED) {
		throw jexception::MemoryException("Creating memory map failed");
	}
}

MemoryMap::~MemoryMap()
{
	munmap(_address, _length);
}

uint8_t * MemoryMap::GetAddress()
{
  return _address;
}

int64_t MemoryMap::GetLength()
{
	return _length;
}

void MemoryMap::SetPermission(jio::jfile_permissions_t perms)
{
	int flags = PROT_NONE;

	if ((perms & jio::JFP_USR_READ) != 0) {
		flags = flags | PROT_READ;
	}

	if ((perms & jio::JFP_USR_WRITE) != 0) {
		flags = flags | PROT_WRITE;
	}

	if ((perms & jio::JFP_USR_EXEC) != 0) {
		flags = flags | PROT_EXEC;
	}

	if (mprotect(_address, _length, flags) < 0) {
		throw jexception::MemoryException("Set permission failed");
	}
}

}
