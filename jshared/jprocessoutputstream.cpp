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
#include "jprocessoutputstream.h"

using namespace std;

namespace jshared {

#ifdef _WIN32
ProcessOutputStream::ProcessOutputStream(HANDLE fd):
#else
ProcessOutputStream::ProcessOutputStream(int fd):
#endif
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::ProcessOutputStream");

	_fd = fd;
}

ProcessOutputStream::~ProcessOutputStream()
{
}

bool ProcessOutputStream::IsEmpty()
{
	return false;
}

int64_t ProcessOutputStream::Available()
{
	return 0LL;
}

int64_t ProcessOutputStream::GetSize()
{
	return 0LL;
}

int64_t ProcessOutputStream::Write(int64_t b)
{
	char byte = (char)b;

	return Write((const char *)&byte, 1LL);
}

int64_t ProcessOutputStream::Write(const char *data, int64_t size)
{
#ifdef _WIN32
	DWORD n;
	
	if (WriteFile(_fd, data, size, &n, NULL) == TRUE) {
		return (int64_t)n;
	}

	return -1LL;
#else
	if (IsBlocking() == true) {
		int64_t n = 0LL;

		if ((n = ::write(_fd, data, size)) > 0) {
			// fsync(_fd);
		}

		return n;
	} else {
		struct timeval waittime;
		fd_set writefs;

		FD_ZERO(&writefs);
		FD_SET(_fd, &writefs);

		waittime.tv_sec = 1L;	// TODO:: usuario especifica ou use pool
		waittime.tv_usec = 0L;

		if (select(_fd+1, NULL, &writefs, NULL, &waittime) < 0) {
			return -1LL;
		}

		int64_t n = 0LL;

		if (FD_ISSET(_fd, &writefs)) {
			if ((n = ::write(_fd, data, size)) > 0) {
				fsync(_fd);
			}
		}

		return n;
	}
#endif
}

int64_t ProcessOutputStream::Write(std::string)
{
	return 0LL;
}

int64_t ProcessOutputStream::Flush()
{
	return 0LL;
}

void ProcessOutputStream::Seek(int64_t index)
{
}

void ProcessOutputStream::Close()
{
#ifdef _WIN32
	CloseHandle(_fd);
#else
	::close(_fd);
#endif
}

bool ProcessOutputStream::IsClosed()
{
	return false;
}

int64_t ProcessOutputStream::GetSentBytes()
{
	return 0LL;
}

}

