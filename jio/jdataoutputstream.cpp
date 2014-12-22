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
#include "jdataoutputstream.h"
#include "jioexception.h"

namespace jio {

DataOutputStream::DataOutputStream(OutputStream *os):
	Object()
{
    jcommon::Object::SetClassName("jio::DataOutputStream");
	
	if ((void *)os == NULL) {
		throw IOException("Null pointer exception");
	}

	_stream = os;
}

DataOutputStream::~DataOutputStream()
{
}

bool DataOutputStream::IsEmpty()
{
	return Available() == 0;
}

int64_t DataOutputStream::Available()
{
	if (_stream != NULL) {
		return _stream->Available();
	}

	return 0LL;
}

int64_t DataOutputStream::GetSize()
{
	return _stream->GetSize();
}

int64_t DataOutputStream::Write(int64_t b)
{
	return _stream->Write(b);
}

int64_t DataOutputStream::Write(const char *data, int64_t size)
{
	return _stream->Write(data, size);
}

void DataOutputStream::WriteBoolean(bool value)
{
	BufferWriter::WriteBoolean(value);
}

void DataOutputStream::WriteByte(uint8_t value)
{
	BufferWriter::WriteByte(value);
}

void DataOutputStream::WriteShort(uint16_t value)
{
	BufferWriter::WriteShort(value);
}

void DataOutputStream::WriteInteger(uint32_t value)
{
	BufferWriter::WriteInteger(value);
}

void DataOutputStream::WriteLong(uint64_t value)
{
	BufferWriter::WriteLong(value);
}

void DataOutputStream::WriteFloat(float value)
{
	BufferWriter::WriteFloat(value);
}

void DataOutputStream::WriteDouble(double value)
{
	BufferWriter::WriteDouble(value);
}

void DataOutputStream::WriteString(std::string value)
{
	BufferWriter::WriteString(value);
}

void DataOutputStream::WriteRaw(const char *data, int64_t size)
{
	BufferWriter::WriteRaw(data, size);
}

void DataOutputStream::Seek(int64_t index)
{
	_stream->Seek(index);
}

int64_t DataOutputStream::Flush()
{
	if (_stream != NULL) {
		std::string data = GetData();

		Write(data.c_str(), data.size());

		return _stream->Flush();
	}

	return 0LL;
}

void DataOutputStream::Close()
{
	if (_stream != NULL) {
		_stream->Close();
	}
}

int64_t DataOutputStream::GetSentBytes()
{
	if (_stream != NULL) {
		_stream->GetSentBytes();
	}

	return 0LL;
}

}

