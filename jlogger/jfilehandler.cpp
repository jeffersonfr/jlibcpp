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
#include "jfilehandler.h"
#include "jloggerexception.h"

#include <sstream>

#include <errno.h>
#include <string.h>

namespace jlogger {

FileHandler::FileHandler(std::string filename_):
	jlogger::StreamHandler()
{
	jcommon::Object::SetClassName("jlogger::FileHandler");

	_file = new jio::File(filename_, (jio::file_flags_t)(jio::F_WRITE_ONLY | jio::F_LARGEFILE | jio::F_APPEND));

	if (_file->Exists() == false) {
		_file = new jio::File(filename_, (jio::file_flags_t)(jio::F_WRITE_ONLY | jio::F_LARGEFILE | jio::F_APPEND | jio::F_CREATE));
	
		if (_file->Exists() == false) {
			throw LoggerException("Error:: cannot create the Log file");
		}
	}
}

FileHandler::~FileHandler()
{
	try {
		_file->Close();
	} catch (...) {
	}
}

void FileHandler::WriteRecord(LogRecord *record_)
{
	_mutex.Lock();
	
	_file->Write(record_->GetRecord().c_str(), record_->GetRecord().size());
	_file->Flush();

	_mutex.Unlock();
}

}

