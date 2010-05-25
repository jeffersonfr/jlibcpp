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
#include "jiolib.h"

namespace jio {

DataOutputStream::DataOutputStream(OutputStream *os):
	Object()
{
    jcommon::Object::SetClassName("jio::DataOutputStream");
	
	if ((void *)os == NULL) {
		throw IOException("Null pointer exception");
	}

	stream = os;
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
	if (stream != NULL) {
		return stream->Available();
	}

	return 0LL;
}

int64_t DataOutputStream::Write(uint8_t data)
{
	if (stream != NULL) {
		return stream->Write((const char *)&data, sizeof(uint8_t));
	}

	return -1LL;
}

int64_t DataOutputStream::Write(uint16_t data)
{
	if (stream != NULL) {
		return stream->Write((const char *)&data, sizeof(uint16_t));
	}

	return -1LL;
}

int64_t DataOutputStream::Write(uint32_t data)
{
	if (stream != NULL) {
		return stream->Write((const char *)&data, sizeof(uint32_t));
	}

	return -1LL;
}

int64_t DataOutputStream::Write(uint64_t data)
{
	if (stream != NULL) {
		return stream->Write((const char *)&data, sizeof(uint64_t));
	}

	return -1LL;
}

int64_t DataOutputStream::Flush()
{
	if (stream != NULL) {
		return stream->Flush();
	}

	return 0LL;
}

void DataOutputStream::Close()
{
	if (stream != NULL) {
		stream->Close();
	}
}

int64_t DataOutputStream::GetSentBytes()
{
	if (stream != NULL) {
		stream->GetSentBytes();
	}

	return 0LL;
}

}

