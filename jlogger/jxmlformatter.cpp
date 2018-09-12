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
#include "jlogger/jxmlformatter.h"

namespace jlogger {

XMLFormatter::XMLFormatter(std::string name_):
	jlogger::Formatter()
{
	jcommon::Object::SetClassName("jlogger::XMLFormatter");

	_name = name_;
	_init = true;
}

XMLFormatter::~XMLFormatter()
{
}

void XMLFormatter::Transform(LogRecord *log)
{
	if (log == nullptr) {
		return;
	}
	
	std::string xml;
		
	if (_init == true) {
		_init = false;

		// xml = "<" + _name + ">\n" + "\t<log type=info>" + log->GetRecord() + "<log>\n";
		if (log->GetType() == JRT_INFO) {
			xml = "<log type=info>" + log->GetRecord() + "<log>\n";
		} else if (log->GetType() == JRT_WARNNING) {
			xml = "<log type=warnning>" + log->GetRecord() + "<log>\n";
		} else if (log->GetType() == JRT_ERROR) {
			xml = "<log type=error>" + log->GetRecord() + "<log>\n";
		} else if (log->GetType() == JRT_CRITICAL) {
			xml = "<log type=critical>" + log->GetRecord() + "<log>\n";
		} else if (log->GetType() == JRT_UNKNOWN) {
			xml = "<log type=unknown>" + log->GetRecord() + "<log>\n";
		}

		log->SetRecord(xml);
	} else {
		if (log->GetType() == JRT_INFO) {
			xml = "<log type=info>" + log->GetRecord() + "</log>\n";
		} else if (log->GetType() == JRT_WARNNING) {
			xml = "<log type=warnning>" + log->GetRecord() + "</log>\n";
		} else if (log->GetType() == JRT_ERROR) {
			xml = "<log type=error>" + log->GetRecord() + "</log>\n";
		} else if (log->GetType() == JRT_CRITICAL) {
			xml = "<log type=critical>" + log->GetRecord() + "</log>\n";
		} else if (log->GetType() == JRT_UNKNOWN) {
			xml = "<log type=unknown>" + log->GetRecord() + "</log>\n";
		}

		log->SetRecord(xml);
	}
}

LogRecord * XMLFormatter::Release()
{
	return nullptr; // new LogRecord(INFO_LOGGER, "</" + _name + ">\n");
}

}
