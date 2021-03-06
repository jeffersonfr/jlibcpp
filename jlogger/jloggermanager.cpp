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
#include "jlogger/jloggermanager.h"
#include "jlogger/jconsolehandler.h"
#include "jlogger/jsimpleformatter.h"

namespace jlogger {

LoggerManager *LoggerManager::_manager = nullptr;

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
  if (_manager == nullptr) {
    _manager = new LoggerManager();
  }

  return _manager;
}
 
Logger * LoggerManager::CreateLogger(LoggerHandler *handler_, Formatter *format_, Logger *group_)
{
  LoggerHandler *h;
  Formatter *f;
    
  h = handler_;
  f = format_;
    
  if (h == nullptr) {
    h = new ConsoleHandler();
  }
   
  if (f == nullptr) {
    f = new SimpleFormatter();
  }
   
  Logger *l = new Logger(h, f);
   
  if (group_ != nullptr) {
    group_->AddLogger(l);
  }
  
  _loggers.push_back(l);
   
  return l;
}

void LoggerManager::SetOutput(int mask_)
{
}

}
