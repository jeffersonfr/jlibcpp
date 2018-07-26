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
#include "jmath/juuid.h"
#include "jexception/jinvalidargumentexception.h"

namespace jmath {

UUID::UUID(juuid_type_t type)
{
	if (type == JUT_RANDOM) {
		uuid_generate_random(_uuid);
	} else if (type == JUT_TIME) {
		uuid_generate_time(_uuid);
	} else if (type == JUT_SYNC) {
		uuid_generate_time_safe(_uuid);
	} else {
		uuid_generate(_uuid);
	}

	_date = jcommon::Date(uuid_time(_uuid, NULL));
}

UUID::UUID(std::string uuid)
{
	if (uuid_parse(uuid.c_str(), _uuid) != 0) {
		throw jexception::InvalidArgumentException("The uuid is not valid");
	}
	
	_date = jcommon::Date(uuid_time(_uuid, NULL));
}

UUID::~UUID()
{
}

jcommon::Date UUID::GetDate()
{
	return _date;
}

jcommon::Object * UUID::Clone()
{
	UUID *uuid = new UUID(JUT_NORMAL);

	uuid->Copy(this);

	return uuid;
}

bool UUID::Equals(Object *o)
{
	UUID *uuid = dynamic_cast<UUID *>(o);

	if (uuid == NULL) {
		return false;
	}

	return (Compare(o) == 0);
}

void UUID::Copy(Object *o)
{
	UUID *uuid = dynamic_cast<UUID *>(o);

	if (uuid == NULL) {
		throw jexception::InvalidArgumentException("The parameter is not a uuid object");
	}

	uuid_copy(_uuid, uuid->_uuid);
}

int UUID::Compare(Object *o)
{
	UUID *uuid = dynamic_cast<UUID *>(o);

	if (uuid == NULL) {
		throw jexception::InvalidArgumentException("The parameter is not a uuid object");
	}

	return uuid_compare(_uuid, uuid->_uuid);
}

std::string UUID::what()
{
	char tmp[256];

	uuid_unparse(_uuid, tmp);

	return tmp;
}

}
