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
#include "jbufferwriter.h"

#include <iostream>

namespace jio {

BufferWriter::BufferWriter() 
{
	jcommon::Object::SetClassName("jio::BufferWriter");
}

BufferWriter::~BufferWriter() 
{
}

std::string BufferWriter::GetData() 
{
	return _buffer.GetString();
}

void BufferWriter::WriteBoolean(bool value) 
{
	_buffer.Put(&value);
}

void BufferWriter::WriteByte(uint8_t value) 
{
	_buffer.Put(&value);
}

void BufferWriter::WriteShort(uint16_t value) 
{
	_buffer.Put(&value);
}

void BufferWriter::WriteInteger(uint32_t value) 
{
	_buffer.Put(&value);
}

void BufferWriter::WriteLong(uint64_t value) 
{
	_buffer.Put(&value);
}

void BufferWriter::WriteFloat(float value) 
{
	_buffer.Put(&value);
}

void BufferWriter::WriteDouble(double value) 
{
	_buffer.Put(&value);
}

void BufferWriter::WriteString(std::string value) 
{
	if (value.empty() == true || value.size() == 0) {
		WriteInteger(0);

		return;
	}

	WriteInteger(value.length());

	_buffer.Put((uint8_t *)value.c_str(), value.length());
}

void BufferWriter::WriteRaw(uint8_t *data, int size) 
{
	if (data == NULL || size <= 0) {
		WriteInteger(0);

		return;
	}
	
	WriteInteger(size);

	_buffer.Put(data, size);
}

void BufferWriter::Reset() 
{
	_buffer.Clear();
}

}
