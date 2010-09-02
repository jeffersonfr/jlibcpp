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
#include "jxmlformatter.h"

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
	if (log == NULL) {
		return;
	}
	
	std::string xml;
		
	if (_init == true) {
		_init = false;

		// xml = "<" + _name + ">\n" + "\t<log type=info>" + log->GetRecord() + "<log>\n";
		if (log->GetType() == INFO_LOGGER) {
			xml = "<log type=info>" + log->GetRecord() + "<log>\n";
		} else if (log->GetType() == WARNNING_LOGGER) {
			xml = "<log type=warnning>" + log->GetRecord() + "<log>\n";
		} else if (log->GetType() == ERROR_LOGGER) {
			xml = "<log type=error>" + log->GetRecord() + "<log>\n";
		} else if (log->GetType() == CRITICAL_LOGGER) {
			xml = "<log type=critical>" + log->GetRecord() + "<log>\n";
		} else if (log->GetType() == UNKNOWN_LOGGER) {
			xml = "<log type=unknown>" + log->GetRecord() + "<log>\n";
		}

		log->SetRecord(xml);
	} else {
		if (log->GetType() == INFO_LOGGER) {
			if (log->GetType() == INFO_LOGGER) {
				xml = "<log type=info>" + log->GetRecord() + "</log>\n";
			} else if (log->GetType() == WARNNING_LOGGER) {
				xml = "<log type=warnning>" + log->GetRecord() + "</log>\n";
			} else if (log->GetType() == ERROR_LOGGER) {
				xml = "<log type=error>" + log->GetRecord() + "</log>\n";
			} else if (log->GetType() == CRITICAL_LOGGER) {
				xml = "<log type=critical>" + log->GetRecord() + "</log>\n";
			} else if (log->GetType() == UNKNOWN_LOGGER) {
				xml = "<log type=unknown>" + log->GetRecord() + "</log>\n";
			}
		}
		
		log->SetRecord(xml);
	}
}

LogRecord * XMLFormatter::Release()
{
	return NULL; // new LogRecord(INFO_LOGGER, "</" + _name + ">\n");
}

}
