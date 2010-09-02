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
#include "jlogrecord.h"

namespace jlogger {

LogRecord::LogRecord(jrecord_type_t type_, std::string record_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jlogger::LogRecord");
	
	_record = record_;
	_type = type_;
}

LogRecord::~LogRecord()
{
}

std::string LogRecord::GetRecord()
{
	return _record;
}

jrecord_type_t LogRecord::GetType()
{
	return _type;
}

void LogRecord::SetRecord(std::string log)
{
	_record = log;
}

void LogRecord::SetType(jrecord_type_t t)
{
	_type = t;
}

}
