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
#include "jmemoryoutputstream.h"

#include "jruntimeexception.h"

#include <iostream>
#include <string>
#include <sstream>

#include <time.h>

namespace jio {

MemoryOutputStream::MemoryOutputStream(unsigned char *data, int size):
	jio::OutputStream()
{
    jcommon::Object::SetClassName("jio::MemoryOutputStream");
	
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

MemoryOutputStream::~MemoryOutputStream()
{
}

bool MemoryOutputStream::IsEmpty()
{
	return Available() == 0;
}

long long MemoryOutputStream::Available()
{
	return (long long)(_buffer_size - _buffer_index);
}

int MemoryOutputStream::Write(int b)
{
	if (IsEmpty() == true) {
		return 0;
	}

	_buffer[_buffer_index++] = (unsigned char)b;

	return 1;
}

long long MemoryOutputStream::Write(char *data, long long size)
{
	if (IsEmpty() == true) {
		return 0LL;
	}

	long long r = size;

	if (r > Available()) {
		r = Available();
	}

	memcpy((unsigned char *)(_buffer + _buffer_index), data, r);

	return r;
}

long long MemoryOutputStream::Flush()
{
	return 0;
}

void MemoryOutputStream::Close()
{
	// DO:: nothing
}

long long MemoryOutputStream::GetSentBytes()
{
	return _buffer_index;
}

}
