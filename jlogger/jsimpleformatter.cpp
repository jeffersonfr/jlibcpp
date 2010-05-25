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
#include "jloggerlib.h"

namespace jlogger {

SimpleFormatter::SimpleFormatter():
	jlogger::Formatter()
{
	jcommon::Object::SetClassName("jlogger::SimpleFormatter");
}

SimpleFormatter::~SimpleFormatter()
{
}

void SimpleFormatter::Transform(LogRecord *log)
{
	if (log == NULL) {
		return;
	}
	
	std::string type;
	
	time_t curtime = time(NULL);
	char *loctime = asctime(localtime (&curtime));
	
	std::ostringstream date,
						format;
		
	if (loctime != NULL) {
		loctime[strlen(loctime)-1] = '\0';
	
		date << " [" << (loctime + 4) << "]  ";
	}
	
	if (log->GetType() == INFO_LOGGER) {
		format << "LOG_INFO" << date.str() << log->GetRecord() << std::flush << std::endl;
	} else if (log->GetType() == WARNNING_LOGGER) {
		format << "LOG_WARNNING" << date.str() << log->GetRecord() << std::flush << std::endl;
	} else if (log->GetType() == ERROR_LOGGER) {
		format << "LOG_ERROR" << date.str() << log->GetRecord() << std::flush << std::endl;
	} else if (log->GetType() == CRITICAL_LOGGER) {
		format << "LOG_CRITICAL" << date.str() << log->GetRecord() << std::flush << std::endl;
	} else if (log->GetType() == UNKNOWN_LOGGER) {
		format << "UNKNOWN" << date.str() << log->GetRecord() << std::flush << std::endl;
	}

	log->SetRecord(format.str());
}

LogRecord * SimpleFormatter::Release()
{
	return NULL;
}

}
