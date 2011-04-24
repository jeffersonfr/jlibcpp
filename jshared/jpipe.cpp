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
#include "jpipe.h"

namespace jshared {

Pipe::Pipe(int size_, int perms_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::Pipe");
		
	_is_closed = false;

#ifdef _WIN32
	if (CreatePipe(&_fdr, &_fdw, 0, size_)  <= 0) {
#else
	if (pipe(_pipe) < 0) {
#endif
		_is_closed = true;
	}
}

Pipe::~Pipe()
{
	Close();
}

int Pipe::Receive(char *data_, int length_)
{
	if (_is_closed == true) {
		return 0;
	}

	long r;

#ifdef _WIN32
	ReadFile(_fdr, data_, length_, (DWORD *)&r, 0);
#else
	r = read(_pipe[0], data_, length_);
#endif

	return r;
}

int Pipe::Send(const char *data_, int length_)
{
	long r;

#ifdef _WIN32
	WriteFile(_fdw, data_, length_, (DWORD *)&r, 0);
#else
	r = write(_pipe[1], data_, length_);
#endif

	return r;
}

bool Pipe::IsClosed()
{
	return _is_closed;
}

void Pipe::Close()
{
	if (_is_closed == false) {
		_is_closed = true;

#ifdef _WIN32
		CloseHandle(_fdr);
		CloseHandle(_fdw);
#else
		close(_pipe[0]);
		close(_pipe[1]);
#endif
	}
}

}
