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

	_buffer.str("");
}

BufferWriter::~BufferWriter() 
{
	_buffer.str("");
}

std::string BufferWriter::GetData() 
{
	return _buffer.str();
}

void BufferWriter::WriteBoolean(bool value) 
{
	WriteByte((value == false)?0:1);
}

void BufferWriter::WriteByte(uint8_t value) 
{
	_buffer.sputn((char *)&value,sizeof(char));
}

void BufferWriter::WriteShort(uint16_t value) 
{
	_buffer.sputn((char *)&value,sizeof(value));
}

void BufferWriter::WriteInteger(uint32_t value) 
{
	_buffer.sputn((char *)&value,sizeof(value));
}

void BufferWriter::WriteLong(uint64_t value) 
{
	_buffer.sputn((char *)&value,sizeof(value));
}

void BufferWriter::WriteFloat(float value) 
{
	_buffer.sputn((char *)&value,sizeof(value));
}

void BufferWriter::WriteDouble(double value) 
{
	_buffer.sputn((char *)&value,sizeof(value));
}

void BufferWriter::WriteString(std::string value) 
{
	WriteInteger(value.length());

	_buffer.sputn(value.c_str(), value.length());
}

void BufferWriter::WriteRaw(uint8_t *data, int size) 
{
	if (data == NULL || size <= 0) {
		WriteInteger(0);
	}
	
	WriteInteger(size);

	_buffer.sputn((char *)data, size);
}

void BufferWriter::Reset() 
{
	_buffer.str("");
}

}
