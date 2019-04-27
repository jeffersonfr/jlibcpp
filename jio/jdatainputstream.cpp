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
#include "jio/jdatainputstream.h"
#include "jexception/jioexception.h"

namespace jio {

DataInputStream::DataInputStream(InputStream *is):
  jio::InputStream(),
  jio::BufferReader()
{
  jcommon::Object::SetClassName("jio::DataInputStream");
    
  if ((void *)is == nullptr) {
    throw jexception::IOException("Null pointer exception");
  }

  _stream = is;
}

DataInputStream::~DataInputStream()
{
}

bool DataInputStream::IsEmpty()
{
  return _stream->IsEmpty();
}

int64_t DataInputStream::Available()
{
  return _stream->Available();
}

int64_t DataInputStream::GetSize()
{
  return _stream->GetSize();
}

int64_t DataInputStream::GetPosition()
{
  return _stream->GetPosition();
}

int64_t DataInputStream::Read()
{
  return _stream->Read();
}

int64_t DataInputStream::Read(char *data, int64_t size)
{
  return _stream->Read(data, size);
}

void DataInputStream::VerifyData()
{
  if (_buffer.Available() > 0) {
    return;
  }

  _buffer.Clear();
  _buffer.Reset();

  char buffer[4096];
  int64_t size = 4096;

  size = _stream->Read(buffer, size);

  if (size > 0) {
    PushData(buffer, size);
  }
}

std::string DataInputStream::Read(int64_t size)
{
  return _stream->Read(size);
}

bool DataInputStream::ReadBoolean()
{
  VerifyData();

  return BufferReader::ReadBoolean();
}

uint8_t DataInputStream::ReadByte()
{
  VerifyData();

  return BufferReader::ReadByte();
}

uint16_t DataInputStream::ReadShort()
{
  VerifyData();

  return BufferReader::ReadShort();
}

uint32_t DataInputStream::ReadInteger()
{
  VerifyData();

  return BufferReader::ReadInteger();
}

uint64_t DataInputStream::ReadLong()
{
  VerifyData();

  return BufferReader::ReadLong();
}

float DataInputStream::ReadFloat()
{
  VerifyData();

  return BufferReader::ReadFloat();
}

double DataInputStream::ReadDouble()
{
  VerifyData();

  return BufferReader::ReadDouble();
}

std::string DataInputStream::ReadString()
{
  VerifyData();

  return BufferReader::ReadString();
}

char * DataInputStream::ReadRaw(int64_t *size)
{
  VerifyData();

  return BufferReader::ReadRaw(size);
}

void DataInputStream::Skip(int64_t skip)
{
  _stream->Skip(skip);
}

void DataInputStream::Reset()
{
  _buffer.Reset();
  _stream->Reset();
}

bool DataInputStream::IsClosed()
{
  return _stream->IsClosed();
}

void DataInputStream::Close()
{
  _stream->Close();
}

int64_t DataInputStream::GetReadedBytes()
{
  return _stream->GetReadedBytes();
}

}

