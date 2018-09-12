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
#include "jio/jmemoryoutputstream.h"
#include "jexception/jruntimeexception.h"

#include <string.h>

namespace jio {

MemoryOutputStream::MemoryOutputStream(const char *data, int64_t size):
	jio::OutputStream()
{
	jcommon::Object::SetClassName("jio::MemoryOutputStream");
	
	if ((void *)data == nullptr) {
		throw jexception::RuntimeException("Null pointer exception");
	}

	_buffer = (char *)data;
	_buffer_size = size;
	_buffer_index = 0;
}

MemoryOutputStream::~MemoryOutputStream()
{
}

bool MemoryOutputStream::IsEmpty()
{
	return Available() == 0LL;
}

int64_t MemoryOutputStream::Available()
{
	return (int64_t)(_buffer_size - _buffer_index);
}

int64_t MemoryOutputStream::Write(int64_t b)
{
	if (IsEmpty() == true) {
		return 0LL;
	}

	_buffer[_buffer_index++] = (uint8_t)b;

	return 1LL;
}

int64_t MemoryOutputStream::Write(const char *data, int64_t size)
{
	if (IsEmpty() == true) {
		return 0LL;
	}

	if (size > Available()) {
		size = Available();
	}

	memcpy((uint8_t*)(_buffer + _buffer_index), data, (uint32_t)size);

	_buffer_index = _buffer_index + size;

	return size;
}

int64_t MemoryOutputStream::Flush()
{
	return 0LL;
}

void MemoryOutputStream::Close()
{
	// DO:: nothing
}

int64_t MemoryOutputStream::GetSentBytes()
{
	return _buffer_index;
}

}
