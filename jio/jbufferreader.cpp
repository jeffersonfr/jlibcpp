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
#include "jbufferreader.h"

#include <stdlib.h>

namespace jio {

BufferReader::BufferReader(uint8_t *data, int size) 
{
	jcommon::Object::SetClassName("jio::BufferReader");

	_buffer.sputn((char *)data, size);
}

BufferReader::~BufferReader() 
{
}

void BufferReader::Reset()
{
	_buffer.pubseekpos(0);
}

void BufferReader::AppendBuffer(uint8_t *data, int size)
{
	_buffer.sputn((char *)data, size);
}

bool BufferReader::ReadBoolean() 
{
	int value = ReadByte();

	return (value == 1);
}

uint8_t BufferReader::ReadByte()
{
	uint8_t value = 0;
	
	_buffer.sgetn((char *)&value, sizeof(value));

	return value;
}

uint16_t BufferReader::ReadShort() 
{
	uint16_t value = 0;

	_buffer.sgetn((char *)&value, sizeof (value));

	return value;
}

uint32_t BufferReader::ReadInteger() 
{
	uint32_t value = 0;

	_buffer.sgetn((char *)&value, sizeof (value));

	return value;
}

uint64_t BufferReader::ReadLong() 
{
	uint64_t value = 0;

	_buffer.sgetn((char *)&value, sizeof (value));

	return value;
}

float BufferReader::ReadFloat() 
{
	float value = 0.0f;

	_buffer.sgetn((char *)&value, sizeof (value));

	return value;
}

double BufferReader::ReadDouble() 
{
	double value = 0.0;

	_buffer.sgetn((char *)&value, sizeof (value));

	return value;
}

std::string BufferReader::ReadString() 
{
	int size = ReadInteger();

	if (size == -1){
		return "";
	}

	char *psz_str = new char[size + 1];

	_buffer.sgetn(psz_str, size+1);

	std::string str = psz_str;

	delete [] psz_str;

	return str;
}

uint8_t * BufferReader::ReadRaw(int *size) 
{
	uint32_t sz = ReadInteger();

	if (sz <= 0) {
		(*size) = 0;
		return NULL;
	}

	uint8_t *data = new uint8_t[sz];

	_buffer.sgetn((char *)data, sz);

	(*size) = sz;

	return data;
}

}

