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
#include "jio/jpipestream.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

namespace jio {

PipeStream::PipeStream(int size):
	jio::InputStream(), jio::OutputStream()
{
	InputStream::SetClassName("jio::PipeStream");
	
	int r;
	
    r = pipe (_fds);
	
	if (r <= 0) {
		_is_open = false;
	}

	_is_open = true;
}

PipeStream::~PipeStream()
{
}

int64_t PipeStream::Read(char *data_, int64_t length_) 
{
	if (_is_open == false) {
		return 0;
	}
	
	return read(_fds[0], data_, length_);
}

int64_t PipeStream::Write(const char *data_, int64_t length_) 
{
	return write(_fds[1], data_, length_);
}

void PipeStream::Close()
{
	close(_fds[0]);
	close(_fds[1]);

	_is_open = false;
}

}

