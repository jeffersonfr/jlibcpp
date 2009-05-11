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
#include "jpipestream.h"
#include "jioexception.h"
#include "jfileexception.h"

#include <iostream>

#include <fcntl.h>
#include <unistd.h>

namespace jio {

PipeStream::PipeStream(int size):
	jio::InputStream(), jio::OutputStream()
{
	InputStream::SetClassName("jio::PipeStream");
	
	int r;
	
#ifdef _WIN32
	r = CreatePipe (&_fdr, &_fdw, 0, size);
#else
    r = pipe (_fds);
#endif
	
	if (r <= 0) {
		_is_open = false;
	}

	_is_open = true;
}

PipeStream::~PipeStream()
{
}

long PipeStream::Read(char *data_, long length_) 
{
	if (_is_open == false) {
		return 0;
	}
	
	long r;
	
#ifdef _WIN32
	    ReadFile(_fdr, data_, length_, (DWORD *)&r, 0);
#else
		r = read(_fds[0], data_, length_);
#endif
	
	return r;
}

long PipeStream::Write(const char *data_, long length_) 
{
	long r;
	
#ifdef _WIN32
	    WriteFile(_fdw, data_, length_, (DWORD *)&r, 0);
#else
		r = write(_fds[1], data_, length_);
#endif

	return r;
}

void PipeStream::Close()
{
#ifdef _WIN32
	CloseHandle(_fdr);
	CloseHandle(_fdw);
#else
	close(_fds[0]);
	close(_fds[1]);
#endif

	_is_open = false;
}

}

