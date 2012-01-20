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
		Close();
	} catch (...) {
	}
}

/** Private */

void LocalServerSocket::CreateSocket()
{
#ifdef _WIN32
#else
	_fd = socket (PF_UNIX, SOCK_STREAM, PF_UNSPEC);

	if (_fd < 0) {
		throw SocketException("ServerSocket handling error");
	}
#endif
}

void LocalServerSocket::BindSocket()
{
#ifdef _WIN32
#else
	int length = sizeof(_address.sun_path)-1;

	unlink(_file.c_str());

	_address.sun_family = AF_UNIX;
	strncpy(_address.sun_path, _file.c_str(), length);
	
	int address_length = sizeof(_address.sun_family) + strnlen(_address.sun_path, length);

	if (bind(_fd, (struct sockaddr *) &_address, address_length) != 0) {
		throw SocketException("ServerBinding error");
	}
#endif
}

void LocalServerSocket::ListenSocket(int backlog_)
{
	if (::listen(_fd, backlog_) < 0) {
		throw SocketException("ServerListen error");
	}
}

/** End */

jsocket_t LocalServerSocket::GetHandler()
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
	struct sockaddr_un address;
	int handler,
			length = sizeof(_address.sun_path)-1;
	socklen_t address_length = strnlen(_file.c_str(), length);
	
	address.sun_family = AF_UNIX;
	strncpy(address.sun_path, _file.c_str(), length);
	
	if ((handler = ::accept(_fd, (struct sockaddr *)&address, &address_length)) < 0) {
		throw SocketException("ServerSocket accept exception");
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
			throw SocketException("Unknown close exception");
		}
	}
	
	unlink(_file.c_str());
#endif
}

}
