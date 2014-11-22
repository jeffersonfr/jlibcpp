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
#include "jnamedpipe.h"
#include "jioexception.h"

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#else
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace jshared {

NamedPipe::NamedPipe(std::string name, int mode):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::NamedPipe");
	
	_name = name;
	
#ifdef _WIN32
	_fd = CreateFileA(
			_name.c_str(),
			GENERIC_WRITE|GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
	);

	if (_fd == INVALID_HANDLE_VALUE) {
		_fd = CreateNamedPipeA(
				_name.c_str(),
				PIPE_ACCESS_DUPLEX,
				// PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE,
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES,
				256,
				256,
				10000,
				NULL
		);

		BOOL ok = ConnectNamedPipe(_fd, NULL);
	} else {
		DWORD err = (_fd == INVALID_HANDLE_VALUE || GetLastError());

		if (err && ((err != ERROR_PIPE_BUSY) || !WaitNamedPipeA(_name.c_str(), NMPWAIT_USE_DEFAULT_WAIT))) {
			throw jio::IOException("Cannot create named pipe");
		}
	}
#else
	if ((_fd = open(_name.c_str(), O_RDWR)) < 0) {
		mkfifo(name.c_str(), mode);
	
		if ((_fd = open(_name.c_str(), O_RDWR)) < 0) {
			throw jio::IOException("Cannot create named pipe");
		}
	}
#endif
	
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

#ifdef _WIN32
	DWORD r;

 	if (ReadFile(_fd, data_, length_, (DWORD *)&r, 0) == FALSE) {
		return -1;
	}

	return r;
#else
	return read(_fd, data_, length_);
#endif
}

int NamedPipe::Send(const char *data_, int length_)
{
#ifdef _WIN32
	DWORD r;

	if (WriteFile(_fd, data_, length_, (DWORD *)&r, 0) == FALSE) {
		return -1;
	}

 	FlushFileBuffers(_fd);

	return r;
#else
	return write(_fd, data_, length_);
#endif
}

bool NamedPipe::IsClosed()
{
	return _is_closed;
}

void NamedPipe::Close()
{
	if (_is_closed == false) {
		_is_closed = true;

#ifdef _WIN32
		CloseHandle(_fd);
#else
		close(_fd);
#endif
	}
}

}
