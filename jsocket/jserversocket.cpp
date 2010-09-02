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
#include "jserversocket.h"
#include "jsocket.h"
#include "junknownhostexception.h"
#include "jsocketexception.h"
#include "jsocketstreamexception.h"

namespace jsocket {

ServerSocket::ServerSocket(int port_, int backlog_, InetAddress *addr_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::ServerSocket");
	
  _local = NULL;
	_is_closed = false;

	if (addr_ == NULL) {
		try {
			InetAddress *a = InetAddress::GetLocalHost();
        
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
		ListenSocket(backlog_);
		
#ifdef _WIN32
		int len;

		if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
#else
		socklen_t len;

		if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
#endif
			throw SocketStreamException("Connect error");
		}
	}
}

ServerSocket::~ServerSocket()
{
	try {
		if (_is_closed == false) {
			_is_closed = true;

    		Close();
		}
	} catch (...) {
	}

    if (_local) {
        delete _local;
    }
}

/** Private */

void ServerSocket::CreateSocket()
{
	_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    
	if (_fd < 0) {
		throw SocketException("Create socket error");
	}
}

void ServerSocket::BindSocket(InetAddress *local_addr_, int local_port_)
{
	bool opt = 1;
    
	_local = local_addr_;
   
	memset(&_lsock, 0, sizeof(_lsock));
    
	_lsock.sin_family = AF_INET;
	_lsock.sin_addr.s_addr = htonl(INADDR_ANY);
	_lsock.sin_port = htons(local_port_);

#ifdef _WIN32
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
#endif
    
	if (::bind(_fd, (struct sockaddr *) &_lsock, sizeof(_lsock)) < 0) {
		throw SocketException("Bind socket error");
	}
}

void ServerSocket::ListenSocket(int backlog_)
{
	if (::listen(_fd, backlog_) < 0) {
		throw SocketException("Listen port error");
	}
}

/** End */

Socket * ServerSocket::Accept()
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
    
	Socket *s = new Socket(handler, _rsock);
    
	return s;
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
