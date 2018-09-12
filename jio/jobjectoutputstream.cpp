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
#include "jio/jobjectoutputstream.h"
#include "jio/jserializable.h"
#include "jexception/jioexception.h"

namespace jio {

ObjectOutputStream::ObjectOutputStream(OutputStream *os):
	Object()
{
	jcommon::Object::SetClassName("jio::ObjectOutputStream");
	
	if ((void *)os == nullptr) {
		throw jexception::IOException("Null pointer exception");
	}

	stream = os;
}

ObjectOutputStream::~ObjectOutputStream()
{
}

bool ObjectOutputStream::IsEmpty()
{
	return Available() == 0;
}

int64_t ObjectOutputStream::Available()
{
	if (stream != nullptr) {
		return stream->Available();
	}

	return 0LL;
}

int64_t ObjectOutputStream::Write(jcommon::Object *object)
{
	if (object == nullptr || stream == nullptr) {
		return -1;
	}

	if (object->InstanceOf("jio::Serializable") == false) {
		return -1;
	}

	jio::Serializable *obj = dynamic_cast<jio::Serializable *>(object);

	std::string name = obj->GetNameSpace();

	if (name != "") {
		name = name + "::";
	}

	name = name + obj->GetClassName();

	return stream->Write("{\"" + name + "\": " + obj->SerializeObject() + "}");
}

int64_t ObjectOutputStream::Flush()
{
	if (stream != nullptr) {
		return stream->Flush();
	}

	return 0LL;
}

void ObjectOutputStream::Close()
{
	if (stream != nullptr) {
		stream->Close();
	}
}

int64_t ObjectOutputStream::GetSentBytes()
{
	if (stream != nullptr) {
		return stream->GetSentBytes();
	}

	return 0LL;
}

}

