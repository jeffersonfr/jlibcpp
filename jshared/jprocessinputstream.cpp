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
#include "jprocessinputstream.h"

using namespace std;

namespace jshared {

ProcessInputStream::ProcessInputStream(int fd):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::ProcessInputStream");
	
	_fd = fd;
}

ProcessInputStream::~ProcessInputStream()
{
}

bool ProcessInputStream::IsEmpty() 
{
	return false;
}

int64_t ProcessInputStream::Available()
{
	return 0LL;
}

int64_t ProcessInputStream::GetSize()
{
	return 0LL;
}

int64_t ProcessInputStream::GetPosition()
{
	return 0LL;
}

int64_t ProcessInputStream::Read()
{
	int64_t r;
	char byte;

	if ((r = Read(&byte, 1LL)) <= 0LL) {
		return -1LL;
	}

	return (int64_t)byte;
}

int64_t ProcessInputStream::Read(char *data, int64_t size)
{
#ifdef _WIN32
	return -1;
#else
	if (IsBlocking() == true) {
		return ::read(_fd, data, size);
	} else {
		struct timeval t;
		fd_set readfs;

		FD_ZERO(&readfs);
		FD_SET(_fd, &readfs);

		t.tv_sec = 1L;
		t.tv_usec = 0L;

		if (select(_fd+1, &readfs, NULL, NULL, &t) < 0) {
			return -1;
		}

		if (FD_ISSET(_fd, &readfs)) {
			return ::read(_fd, data, size);
		}
	}

	return -1;
#endif
}

void ProcessInputStream::Skip(int64_t skip)
{
}

void ProcessInputStream::Reset()
{
}

void ProcessInputStream::Close()
{
	::close(_fd);
}

bool ProcessInputStream::IsClosed()
{
	return false;
}

int64_t ProcessInputStream::GetReceiveBytes()
{
	return 0LL;
}

}
