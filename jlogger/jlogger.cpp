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
#include "jlogger/jlogger.h"

#include <stdarg.h>

namespace jlogger {

Logger::Logger(LoggerHandler *handler_, Formatter *format_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jlogger::Logger");
	
	_handler = handler_;
	_format = format_;
	_mask = (jrecord_type_t)(JRT_INFO | JRT_WARNNING | JRT_ERROR | JRT_CRITICAL | JRT_UNKNOWN);
}

Logger::~Logger()
{
	if (_handler != NULL) {
		delete _handler;
	}

	if (_format != NULL) {
		delete _format;
	}
}

void Logger::AddLogger(Logger *logger)
{
	_loggers.push_back(logger);
}

void Logger::SendLogger(jrecord_type_t type_, std::string record_)
{
	if ((_mask & type_) == 0) {
		return;
	}

	LogRecord *record = NULL;
    
	try {
		record = new LogRecord(type_, record_);

		if (_format != NULL) {
			_format->Transform(record);
		}
		
		_handler->WriteRecord(record);

		delete record;
	} catch (std::bad_alloc &) {
		if (record != NULL) {
			delete record;
		}
	}
}

void Logger::SendLogger(jrecord_type_t type_, const char *fmt, ...)
{
	if ((_mask & type_) == 0) {
		return;
	}

	char tmp[4096];
	va_list va;

	va_start(va, fmt);
	vsnprintf(tmp, 4096, fmt, va); tmp[4096-1] = 0;
	va_end(va);

	LogRecord *record = NULL;
        
	try {
		record = new LogRecord(type_, tmp);

		if (_format != NULL) {
			_format->Transform(record);
		}
		
		_handler->WriteRecord(record);

		delete record;
	} catch (std::bad_alloc &) {
		if (record != NULL) {
			delete record;
		}
	}
}

void Logger::SendLogger(LogRecord *record_)
{
	if ((_mask & record_->GetType()) == 0) {
		return;
	}

	try {
		if (_format != NULL) {
			_format->Transform(record_);
		}
		
		_handler->WriteRecord(record_);
	} catch (...) {
	}
}

void Logger::SetOutput(int mask_)
{
	_mask = (jrecord_type_t)mask_;
}

void Logger::SetHandler(LoggerHandler *handler_)
{
	if (handler_ != NULL) {
		if (_handler != NULL) {
			delete _handler;
		}
        
		_handler = handler_;
	}
}

void Logger::SetFormatter(Formatter *format_)
{
	if (format_ != NULL) {
		if (_format != NULL) {
			delete _format;
		}
		
		_format = format_;
	}
}

void Logger::Release()
{
	LogRecord *record = NULL;
        
	try {
		if (_format != NULL) {
			record = _format->Release();
		}
	
		if (record != NULL) {	
			_handler->WriteRecord(record);

			delete record;
		}
	} catch (std::bad_alloc &) {
		if (record != NULL) {
			delete record;
		}
	}
}

}
