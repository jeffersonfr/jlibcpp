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
#include "jio/jbufferreader.h"

#include <stdlib.h>

namespace jio {

BufferReader::BufferReader() 
{
	jcommon::Object::SetClassName("jio::BufferReader");
}

BufferReader::BufferReader(char *data, int64_t size) 
{
	jcommon::Object::SetClassName("jio::BufferReader");

	_buffer.Put(data, size);
}

BufferReader::~BufferReader() 
{
}

void BufferReader::PushData(char *data, int64_t size)
{
	_buffer.Put(data, size);
}

bool BufferReader::ReadBoolean() 
{
	bool byte;

	_buffer.Get(&byte);

	return byte;
}

uint8_t BufferReader::ReadByte()
{
	bool byte;

	_buffer.Get(&byte);

	return byte;
}

uint16_t BufferReader::ReadShort() 
{
	uint16_t n;

	_buffer.Get(&n);

	return n;
}

uint32_t BufferReader::ReadInteger() 
{
	uint32_t n;

	_buffer.Get(&n);

	return n;
}

uint64_t BufferReader::ReadLong() 
{
	uint64_t n;

	_buffer.Get(&n);

	return n;
}

float BufferReader::ReadFloat() 
{
	float n;

	_buffer.Get(&n);

	return n;
}

double BufferReader::ReadDouble() 
{
	double n;

	_buffer.Get(&n);

	return n;
}

std::string BufferReader::ReadString() 
{
	uint32_t sz = ReadInteger();

	if (sz <= 0){
		return "";
	}

	char *psz_str = new char[sz];

	_buffer.Get(psz_str, sz);

	std::string str = std::string(psz_str, sz);

	delete [] psz_str;

	return str;
}

char * BufferReader::ReadRaw(int64_t *size) 
{
	uint32_t sz = ReadInteger();

	if (sz <= 0) {
		(*size) = 0;
		return NULL;
	}

	char *data = new char[sz];

	_buffer.Get(data, sz);

	(*size) = sz;

	return data;
}

void BufferReader::Reset()
{
	_buffer.Reset();
}

}

