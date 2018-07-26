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
#include "jio/jmemoryinputstream.h"
#include "jexception/jruntimeexception.h"

#include <string.h>

namespace jio {

MemoryInputStream::MemoryInputStream(const uint8_t *data, int64_t size):
	jio::InputStream()
{
	jcommon::Object::SetClassName("jio::MemoryInputStream");

	if ((void *)data == NULL) {
		throw jexception::RuntimeException("Null pointer exception");
	}

	_buffer = (char *)data;
	_buffer_size = size;
	_buffer_index = 0;
}

MemoryInputStream::~MemoryInputStream()
{
}

bool MemoryInputStream::IsEmpty()
{
	return _buffer_size == _buffer_index;
}

int64_t MemoryInputStream::Available()
{
	return _buffer_size - _buffer_index;
}

int64_t MemoryInputStream::GetSize()
{
	return _buffer_size;
}

int64_t MemoryInputStream::GetPosition()
{
	return _buffer_index;
}

int64_t MemoryInputStream::Read()
{
	if (IsEmpty() == true) {
		return -1LL;
	}

	return _buffer[_buffer_index++];;
}

int64_t MemoryInputStream::Read(char *data, int64_t size)
{
	if ((void *)data == NULL) {
		return -1LL;
	}

	if (IsEmpty() == true) {
		return -1LL;
	}

	if (size > Available()) {
		size = Available();
	}

	memcpy(data, _buffer + _buffer_index, (size_t)size);

	_buffer_index = _buffer_index + size;

	return size;
}

void MemoryInputStream::Skip(int64_t skip)
{
	_buffer_index += skip;

	if (_buffer_index > Available()) {
		_buffer_index = Available();
	}
}

void MemoryInputStream::Reset()
{
	_buffer_index = 0;
}

void MemoryInputStream::Close()
{
}

int64_t MemoryInputStream::GetReadedBytes()
{
	return _buffer_index;
}

}
