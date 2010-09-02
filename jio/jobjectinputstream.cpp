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
#include "jobjectinputstream.h"
#include "jioexception.h"
#include "jserializable.h"

namespace jio {

ObjectInputStream::ObjectInputStream(InputStream *is):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jio::ObjectInputStream");
	
	if ((void *)is == NULL) {
		throw IOException("Null pointer exception");
	}

	stream = is;
}

ObjectInputStream::~ObjectInputStream()
{
}

bool ObjectInputStream::IsEmpty()
{
	return Available() == 0;
}

int64_t ObjectInputStream::Available()
{
	if (stream != NULL) {
		return stream->Available();
	}

	return 0LL;
}

int64_t ObjectInputStream::GetSize()
{
	if (stream != NULL) {
		return stream->GetSize();
	}

	return 0LL;
}

int64_t ObjectInputStream::GetPosition()
{
	return 0LL;
}

jcommon::Object * ObjectInputStream::Read()
{
	if (stream == NULL) {
		return NULL;
	}

	jcommon::Object *object = NULL;
	std::string description;
	char *ptr,
		 tmp[65536];

	if (stream->Read(tmp, 65536) <= 0) {
		return NULL;
	}

	ptr = strchr(tmp+2, '\"');

	if (ptr != NULL) {
		*ptr = '\0';
		*(ptr+strlen(ptr+1)) = '\0';

		description = (ptr+3);
	} else {
		ptr = (char *)"";
	}
	
	object = CreateObject((tmp+2));

	if (object != NULL) {
		if (object->InstanceOf("jio::Serializable") == true) {
			((jio::Serializable *)object)->AssemblyObject(description);
		} else {
			delete object;
			object = NULL;
		}

	}

	return object;
}

void ObjectInputStream::Skip(int64_t skip)
{
	if (stream != NULL) {
		stream->Skip(skip);
	}
}

void ObjectInputStream::Reset()
{
	if (stream != NULL) {
		stream->Reset();
	}
}

void ObjectInputStream::Close()
{
	if (stream != NULL) {
		stream->Close();
	}
}

int64_t ObjectInputStream::GetReceiveBytes()
{
	if (stream != NULL) {
		return stream->GetReceiveBytes();
	}

	return 0LL;
}

jcommon::Object * ObjectInputStream::CreateObject(std::string id)
{
	return NULL;
}

}

