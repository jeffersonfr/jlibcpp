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
#include "jshared/jnamedpipe.h"
#include "jexception/jioexception.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

namespace jshared {

NamedPipe::NamedPipe(std::string name, int mode):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::NamedPipe");
	
	_name = name;
	
	if ((_fd = open(_name.c_str(), O_RDWR)) < 0) {
		mkfifo(name.c_str(), mode);
	
		if ((_fd = open(_name.c_str(), O_RDWR)) < 0) {
			throw jexception::IOException("Cannot create named pipe");
		}
	}
	
	_is_closed = false;
}

NamedPipe::~NamedPipe()
{
	Close();
}

int NamedPipe::Receive(char *data_, int length_)
{
	if (_is_closed == true) {
		return 0;
	}

	return read(_fd, data_, length_);
}

int NamedPipe::Send(const char *data_, int length_)
{
	return write(_fd, data_, length_);
}

bool NamedPipe::IsClosed()
{
	return _is_closed;
}

void NamedPipe::Close()
{
	if (_is_closed == false) {
		_is_closed = true;

		close(_fd);
	}
}

}
