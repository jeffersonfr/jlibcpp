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
#include "jmemoryinputstream.h"

#include "jruntimeexception.h"

#include <iostream>
#include <string>
#include <sstream>

#include <time.h>

namespace jio {

MemoryInputStream::MemoryInputStream(uint8_t*data, int size):
	jio::InputStream()
{
    jcommon::Object::SetClassName("jio::MemoryInputStream");

	if ((void *)data == NULL) {
		throw jcommon::RuntimeException("Null pointer exception");
	}

	if (size <= 0) {
		throw jcommon::RuntimeException("Size is out of range");
	}

	_buffer = data;
	_buffer_size = size;
	_buffer_index = 0;
}

MemoryInputStream::~MemoryInputStream()
{
}

bool MemoryInputStream::IsEmpty()
{
	return Available() <= 0;
}

int64_t MemoryInputStream::Available()
{
	return (int64_t)(_buffer_size - _buffer_index);
}

int64_t MemoryInputStream::GetSize()
{
	return _buffer_size;
}

int64_t MemoryInputStream::GetPosition()
{
	return _buffer_index;
}

int MemoryInputStream::Read()
{
	if (IsEmpty() == true) {
		return -1;
	}

	return _buffer[_buffer_index++];;
}

int64_t MemoryInputStream::Read(char *data, int64_t size)
{
	if (IsEmpty() == true) {
		return -1LL;
	}

	int64_t r = size;

	if (r > Available()) {
		r = Available();
	}

	memcpy(data, (uint8_t*)(_buffer + _buffer_index), r);

	return r;
}

void MemoryInputStream::Skip(int64_t skip)
{
	int64_t r = skip;

	if (r > Available()) {
		r = Available();
	}

	_buffer_index += r;
}

void MemoryInputStream::Reset()
{
	_buffer_index = 0;
}

void MemoryInputStream::Close()
{
}

int64_t MemoryInputStream::GetReceiveBytes()
{
	return _buffer_index;
}

}
