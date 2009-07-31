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

MemoryInputStream::MemoryInputStream(unsigned char *data, int size):
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

long long MemoryInputStream::Available()
{
	return (long long)(_buffer_size - _buffer_index);
}

long long MemoryInputStream::GetSize()
{
	return _buffer_size;
}

long long MemoryInputStream::GetPosition()
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

long long MemoryInputStream::Read(char *data, long long size)
{
	if (IsEmpty() == true) {
		return -1LL;
	}

	long long r = size;

	if (r > Available()) {
		r = Available();
	}

	memcpy(data, (unsigned char *)(_buffer + _buffer_index), r);

	return r;
}

void MemoryInputStream::Skip(long long skip)
{
	long long r = skip;

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

long long MemoryInputStream::GetReceiveBytes()
{
	return _buffer_index;
}

}
