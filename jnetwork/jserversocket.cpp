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
#include "jnetwork/jserversocket.h"
#include "jnetwork/jsocket.h"
#include "jnetwork/jinetaddress4.h"
#include "jexception/junknownhostexception.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jioexception.h"

#include <string.h>
#include <unistd.h>

namespace jnetwork {

ServerSocket::ServerSocket(int port_, int backlog_, InetAddress *addr_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jnetwork::ServerSocket");
	
  _local = NULL;
	_is_closed = true;

	CreateSocket();

	BindSocket(addr_, port_);
	ListenSocket(backlog_);

	if (port_ == 0) {
		socklen_t len;

		len = sizeof(_lsock);

		if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
			throw jexception::IOException("ServerSocket constructor exception");
		}
	}
}

ServerSocket::~ServerSocket()
{
	try {
		Close();
	} catch (...) {
	}

	if (_local != NULL) {
		delete _local;
	}
}

/** Private */

void ServerSocket::CreateSocket()
{
	if ((_fd = ::socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		throw jexception::ConnectionException("ServerSocket handling error");
	}

	_is_closed = false;
}

void ServerSocket::BindSocket(InetAddress *local_addr_, int local_port_)
{
	bool opt = true;
    
	memset(&_lsock, 0, sizeof(_lsock));
    
	_lsock.sin_family = AF_INET;

	if (local_addr_ == NULL) {
		_local = InetAddress4::GetLocalHost();

		_lsock.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		_local = dynamic_cast<InetAddress4 *>(local_addr_);

		_lsock.sin_addr.s_addr = inet_addr(_local->GetHostAddress().c_str());
		// memcpy(&(_lsock.sin_addr.s_addr), &(_local->_ip), sizeof(_local->_ip));
	}

	_lsock.sin_port = htons(local_port_);

	/*
	#ifndef SO_REUSEPORT
		#define SO_REUSEPORT 15
	#endif
	
	setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, (void *)&opt, sizeof(opt));
	*/
	
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
    
	if (::bind(_fd, (struct sockaddr *) &_lsock, sizeof(_lsock)) < 0) {
		throw jexception::ConnectionException("ServerBinding error");
	}
}

void ServerSocket::ListenSocket(int backlog_)
{
	if (::listen(_fd, backlog_) < 0) {
		throw jexception::ConnectionException("ServerListen error");
	}
}

/** End */

Socket * ServerSocket::Accept()
{
	socklen_t sock_size;
	int handler;
	
	sock_size = sizeof(_rsock);

	handler = ::accept(_fd, (struct sockaddr *) &_rsock, &sock_size);
    
	if (handler < 0) {
		throw jexception::ConnectionException("Socket accept exception");
	}
    
	return new Socket(handler, _rsock);
}

InetAddress * ServerSocket::GetInetAddress()
{
	return _local;
}

int ServerSocket::GetLocalPort()
{
	return ntohs(_lsock.sin_port);
}

void ServerSocket::Close()
{
	if (_is_closed == true) {
		return;
	}

	if (close(_fd) != 0) {
		throw jexception::ConnectionException("Unknown close exception");
	}

	_is_closed = true;
}

bool ServerSocket::IsClosed()
{
	return _is_closed;
}

}
