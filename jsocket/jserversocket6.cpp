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
#include "jserversocket6.h"
#include "jsocket6.h"
#include "junknownhostexception.h"
#include "jsocketexception.h"
#include "jioexception.h"
#include "jinetaddress6.h"

namespace jsocket {

ServerSocket6::ServerSocket6(int port_, int backlog_, InetAddress *addr_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::ServerSocket6");
	
#ifdef _WIN32
#else
	_local = NULL;
	_is_closed = true;

	CreateSocket();

	if (port_ != 0) {
		BindSocket(addr_, port_);
		ListenSocket(backlog_);
	} else {
		socklen_t len = sizeof(_lsock);
		
		ListenSocket(backlog_);

		if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
			throw jio::IOException("ServerSocket constructor exception");
		}
	}
#endif
}

ServerSocket6::~ServerSocket6()
{
#ifdef _WIN32
#else
	try {
		Close();
	} catch (...) {
	}

	if (_local != NULL) {
		delete _local;
	}
#endif
}

/** Private */

void ServerSocket6::CreateSocket()
{
#ifdef _WIN32
#else
	if ((_fd = ::socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
		throw SocketException("ServerSocket Creation exception");
	}

	_is_closed = false;
#endif
}

void ServerSocket6::BindSocket(InetAddress *local_addr_, int local_port_)
{
#ifdef _WIN32
#else
	bool opt = 1;
    
	memset(&_lsock, 0, sizeof(_lsock));
    
	_lsock.sin6_family = AF_INET6;
	_lsock.sin6_flowinfo = 0;
	_lsock.sin6_scope_id = 0;

	if (local_addr_ == NULL) {
		_local = dynamic_cast<InetAddress6 *>(InetAddress6::GetLocalHost());

		_lsock.sin6_addr = in6addr_any;
	} else {
		_local = dynamic_cast<InetAddress6 *>(local_addr_);

		memcpy(&(_lsock.sin6_addr), &(_local->_ip), sizeof(_local->_ip));
	}

	_lsock.sin6_port = htons(local_port_);

	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
    
	if (::bind(_fd, (struct sockaddr *) &_lsock, sizeof(_lsock)) < 0) {
		throw SocketException("ServerBinding error");
	}
#endif
}

void ServerSocket6::ListenSocket(int backlog_)
{
#ifdef _WIN32
#else
	if (::listen(_fd, backlog_) < 0) {
		throw SocketException("ServerListen error");
	}
#endif
}

/** End */

Socket6 * ServerSocket6::Accept()
{
#ifdef _WIN32
	return NULL;
#else
	socklen_t sock_size;
	int handler;
	
	sock_size = sizeof(_rsock);

	handler = ::accept(_fd, (struct sockaddr *) &_rsock, &sock_size);
    
	if (handler < 0) {
		throw SocketException("Socket accept exception");
	}

	return new Socket6(handler, _rsock);
#endif
}

InetAddress * ServerSocket6::GetInetAddress()
{
	return _local;
}

int ServerSocket6::GetLocalPort()
{
#ifdef _WIN32
	return 0;
#else
	return ntohs(_lsock.sin6_port);
#endif
}

void ServerSocket6::Close()
{
#ifdef _WIN32
#else
	if (_is_closed == true) {
		return;
	}

	if (close(_fd) != 0) {
		throw SocketException("Unknow Close exception");
	}

	_is_closed = true;
#endif
}

bool ServerSocket6::IsClosed()
{
	return _is_closed;
}

}
