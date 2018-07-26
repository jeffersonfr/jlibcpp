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
#include "jnetwork/jlocalserversocket.h"
#include "jnetwork/jsocket.h"
#include "jexception/junknownhostexception.h"
#include "jexception/jconnectionexception.h"

#include <unistd.h>

namespace jnetwork {

LocalServerSocket::LocalServerSocket(std::string file, int backlog_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jnetwork::ServerSocket");
	
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
	_fd = socket (PF_UNIX, SOCK_STREAM, PF_UNSPEC);

	if (_fd < 0) {
		throw jexception::ConnectionException("ServerSocket handling error");
	}
}

void LocalServerSocket::BindSocket()
{
	int length = sizeof(_address.sun_path)-1;

	unlink(_file.c_str());

	_address.sun_family = AF_UNIX;
	strncpy(_address.sun_path, _file.c_str(), length);
	
	int address_length = sizeof(_address.sun_family) + strnlen(_address.sun_path, length);

	if (bind(_fd, (struct sockaddr *) &_address, address_length) != 0) {
		throw jexception::ConnectionException("ServerBinding error");
	}
}

void LocalServerSocket::ListenSocket(int backlog_)
{
	if (::listen(_fd, backlog_) < 0) {
		throw jexception::ConnectionException("ServerListen error");
	}
}

/** End */

LocalSocket * LocalServerSocket::Accept()
{
	struct sockaddr_un address;
	int handler,
			length = sizeof(_address.sun_path)-1;
	socklen_t address_length = strnlen(_file.c_str(), length);
	
	address.sun_family = AF_UNIX;
	strncpy(address.sun_path, _file.c_str(), length);
	
	if ((handler = ::accept(_fd, (struct sockaddr *)&address, &address_length)) < 0) {
		throw jexception::ConnectionException("ServerSocket accept exception");
	}

	return new LocalSocket(handler, _file);
}

std::string LocalServerSocket::GetServerFile()
{
	return _file;
}

void LocalServerSocket::Close()
{
	if (_is_closed == false) {
		_is_closed = true;

		if (close(_fd) != 0) {
			throw jexception::ConnectionException("Unknown close exception");
		}
	}
	
	unlink(_file.c_str());
}

}
