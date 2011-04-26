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
#include "jsocketstreamexception.h"
#include "jinetaddress6.h"

namespace jsocket {

ServerSocket6::ServerSocket6(int port_, int backlog_, InetAddress *addr_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::ServerSocket6");
	
  _local = NULL;
	_is_closed = false;

	if (addr_ == NULL) {
		try {
			InetAddress *a = InetAddress6::GetLocalHost();
        
			addr_ = a;
		} catch (UnknownHostException &) {
			addr_ = NULL;
		}
	}

	CreateSocket();

	if (port_ != 0) {
		BindSocket(addr_, port_);
		ListenSocket(backlog_);
	} else {
#ifdef _WIN32
		int len;
#else
		socklen_t len;
#endif
		
		ListenSocket(backlog_);
		
		if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
			throw SocketStreamException("Connect error");
		}
	}
}

ServerSocket6::~ServerSocket6()
{
	try {
		Close();
	} catch (...) {
	}

	if (_local) {
		delete _local;
	}
}

/** Private */

void ServerSocket6::CreateSocket()
{
	if ((_fd = ::socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
		throw SocketException("Create socket error");
	}
}

void ServerSocket6::BindSocket(InetAddress *local_addr_, int local_port_)
{
	bool opt = 1;
    
	_local = local_addr_;
   
	memset(&_lsock, 0, sizeof(_lsock));
    
	_lsock.sin6_family = AF_INET6;
	_lsock.sin6_flowinfo = 0;
	_lsock.sin6_scope_id = 0;
	_lsock.sin6_addr = in6addr_any;
	_lsock.sin6_port = htons(local_port_);

#ifdef _WIN32
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
#endif
    
	if (::bind(_fd, (struct sockaddr *) &_lsock, sizeof(_lsock)) < 0) {
		throw SocketException("Bind socket error");
	}
}

void ServerSocket6::ListenSocket(int backlog_)
{
	if (::listen(_fd, backlog_) < 0) {
		throw SocketException("Listen port error");
	}
}

/** End */

Socket6 * ServerSocket6::Accept()
{
#ifdef _WIN32
	int sock_size;
	int handler;
	
	sock_size = sizeof(_rsock);
	handler = ::accept(_fd, (struct sockaddr *) &_rsock, &sock_size);
#else 
	socklen_t sock_size;
	int handler;
	
	sock_size = sizeof(_rsock);
	handler = ::accept(_fd, (struct sockaddr *) &_rsock, &sock_size);
#endif
    
	if (handler < 0) {
		throw SocketException("Accept failed");
	}

	Socket6 *s = new Socket6(handler, _rsock);
    
	return s;
}

InetAddress * ServerSocket6::GetInetAddress()
{
	return _local;
}

int ServerSocket6::GetLocalPort()
{
	return ntohs(_lsock.sin6_port);
}

void ServerSocket6::Close()
{
#ifdef _WIN32
	if (_is_closed == false) {
		_is_closed = true;

		if (closesocket(_fd) < 0) {
			throw SocketException("Close socket error");
		}
	}
#else
	if (_is_closed == false) {
		_is_closed = true;

		if (close(_fd) != 0) {
			throw SocketException("Close socket error");
		}
	}
#endif
}

}
