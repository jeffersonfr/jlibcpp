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
#include "jdatainputstream.h"
#include "jioexception.h"

namespace jio {

DataInputStream::DataInputStream(InputStream *is):
	Object()
{
	jcommon::Object::SetClassName("jio::DataInputStream");
		
	if ((void *)is == NULL) {
		throw IOException("Null pointer exception");
	}

	stream = is;
}

DataInputStream::~DataInputStream()
{
}

bool DataInputStream::IsEmpty()
{
	return Available() == 0;
}

int64_t DataInputStream::Available()
{
	return stream->Available();
}

int64_t DataInputStream::GetSize()
{
	if (stream != NULL) {
		stream->GetSize();
	}

	return 0LL;
}

int64_t DataInputStream::GetPosition()
{
	if (stream != NULL) {
		stream->GetPosition();
	}

	return 0LL;
}

int DataInputStream::Read(uint8_t *data)
{
	if (stream != NULL) {
		stream->Read((char *)data, sizeof(uint8_t));
	}

	return -1;
}

int DataInputStream::Read(uint16_t *data)
{
	if (stream != NULL) {
		stream->Read((char *)data, sizeof(uint16_t));
	}

	return -1;
}

int DataInputStream::Read(uint32_t *data)
{
	if (stream != NULL) {
		stream->Read((char *)data, sizeof(uint32_t));
	}

	return -1;
}

int DataInputStream::Read(uint64_t *data)
{
	if (stream != NULL) {
		stream->Read((char *)data, sizeof(uint64_t));
	}

	return -1;
}

void DataInputStream::Skip(int64_t skip)
{
	if (stream != NULL) {
		stream->Skip(skip);
	}
}

void DataInputStream::Reset()
{
	if (stream != NULL) {
		stream->Reset();
	}
}

void DataInputStream::Close()
{
	if (stream != NULL) {
		stream->Close();
	}
}

int64_t DataInputStream::GetReceiveBytes()
{
	if (stream != NULL) {
		return stream->GetReceiveBytes();
	}

	return 0LL;
}

}

