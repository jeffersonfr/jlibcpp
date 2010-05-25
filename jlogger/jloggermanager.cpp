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

LoggerManager *LoggerManager::_manager = NULL;

LoggerManager::LoggerManager():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jlogger::LoggerManager");
}

LoggerManager::~LoggerManager()
{
}

LoggerManager * LoggerManager::GetInstance()
{
	if (_manager == NULL) {
		_manager = new LoggerManager();
	}

	return _manager;
}
 
Logger * LoggerManager::CreateLogger(Handler *handler_, Formatter *format_, Logger *group_)
{
	Handler *h;
	Formatter *f;
    
	h = handler_;
	f = format_;
    
	if (h == NULL) {
		h = new ConsoleHandler();
	}
   
	if (f == NULL) {
		f = new SimpleFormatter();
	}
   
	Logger *l = new Logger(h, f);
   
	if (group_ != NULL) {
		group_->AddLogger(l);
	}
	
	_loggers.push_back(l);
   
	return l;
}

void LoggerManager::SetOutput(int mask_)
{
}

}
