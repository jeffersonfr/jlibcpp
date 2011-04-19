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
#include "jlocalserversocket.h"
#include "jsocket.h"
#include "junknownhostexception.h"
#include "jsocketexception.h"
#include "jsocketstreamexception.h"

namespace jsocket {

LocalServerSocket::LocalServerSocket(std::string file, int backlog_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::ServerSocket");
	
	_file = file;
	_is_closed = false;

	CreateSocket();
	BindSocket();
	ListenSocket(backlog_);
}

LocalServerSocket::~LocalServerSocket()
{
	try {
		if (_is_closed == false) {
			_is_closed = true;

			Close();
		}
	} catch (...) {
	}
}

/** Private */

void LocalServerSocket::CreateSocket()
{
#ifdef _WIN32
#else
	_fd = socket (PF_UNIX, SOCK_STREAM, 0); // IPPROTO_TCP);

	if (_fd < 0) {
		throw SocketException("Create socket error");
	}
#endif
}

void LocalServerSocket::BindSocket()
{
#ifdef _WIN32
#else
	unlink(_file.c_str());

	_address.sun_family = PF_UNIX;
	strncpy(_address.sun_path, _file.c_str(), 255);
	
	int address_length = sizeof(_address.sun_family) + strnlen(_address.sun_path, 255);

	if (bind(_fd, (struct sockaddr *) &_address, address_length) != 0) {
		throw SocketException("Bind socket error");
	}
#endif
}

void LocalServerSocket::ListenSocket(int backlog_)
{
	if (::listen(_fd, backlog_) < 0) {
		throw SocketException("Listen port error");
	}
}

/** End */

#ifdef _WIN32
SOCKET LocalServerSocket::GetHandler()
#else
int LocalServerSocket::GetHandler()
#endif
{
#ifdef _WIN32
	return -1;
#else
	return _fd;
#endif
}

LocalSocket * LocalServerSocket::Accept()
{
#ifdef _WIN32
	return NULL;
#else 
	sockaddr_un address;
	socklen_t address_length;
	int handler;
	
	if ((handler = ::accept(_fd, (struct sockaddr *)&address, &address_length)) < 0) {
		throw SocketException("Accept failed");
	}

	return new LocalSocket(handler, _file);
#endif
}

std::string LocalServerSocket::GetServerFile()
{
	return _file;
}

void LocalServerSocket::Close()
{
#ifdef _WIN32
#else
	if (_is_closed == false) {
		_is_closed = true;

		if (close(_fd) != 0) {
			throw SocketException("Close socket error");
		}
	}
	
	unlink(_file.c_str());
#endif
}

}
