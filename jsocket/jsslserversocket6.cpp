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
#include "jsslserversocket6.h"
#include "jsocketexception.h"
#include "jioexception.h"
#include "jinetaddress6.h"

namespace jsocket {

SSLServerSocket6::SSLServerSocket6(SSLContext *ctx, int port_, int backlog_, InetAddress *addr_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::SSLServerSocket6");

	_local = NULL;
	_is_closed = true;

	_ctx = ctx;

	CreateSocket();

	if (port_ != 0) {
		BindSocket(addr_, port_);
		ListenSocket(backlog_);
	} else {
		ListenSocket(backlog_);
		
		socklen_t len;

		len = sizeof(_lsock);

		if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
			throw jio::IOException("ServerSocket constructor exception");
		}
	}
}

SSLServerSocket6::~SSLServerSocket6()
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

void SSLServerSocket6::CreateSocket()
{
	if ((_fd = ::socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		throw SocketException("ServerSocket handling error");
	}

	_is_closed = false;
}

void SSLServerSocket6::BindSocket(InetAddress *local_addr_, int local_port_)
{
	int opt = 1;
    
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
}

void SSLServerSocket6::ListenSocket(int backlog_)
{
	if (::listen(_fd, backlog_) < 0) {
		throw SocketException("ServerListen error");
	}
}

/** End */

SSLSocket6 * SSLServerSocket6::Accept()
{
	socklen_t sock_size;
	int handler;
	
	sock_size = sizeof(_rsock);

	handler = ::accept(_fd, (struct sockaddr *) &_rsock, &sock_size);
    
	if (handler < 0) {
		throw SocketException("ServerSocket accept exception");
	}
    
	/*
	if (_blocked == false) {
		fcntl(handler, F_SETFL, O_NONBLOCK);
	}
	*/

	SSL *ssl = SSL_new(_ctx->GetSSLContext());

	if (!ssl) {
		throw SocketException("ServerSocket cannot create secure handler exception");
	}

	if (SSL_set_fd(ssl, handler) < 1) {
		SSL_shutdown(ssl);
		SSL_free(ssl);

		throw SocketException("ServerSocket cannot set secure handler exception");
	}

	SSL_set_accept_state(ssl);

	if (SSL_accept(ssl) < 1) {
		SSL_shutdown(ssl);
		SSL_free(ssl);

		throw SocketException("ServerSocket accept secure handshake exception");
	}

	if (SSL_get_verify_result(ssl) != X509_V_OK) {
		throw SocketException("ServerSocket certificate failed verification");
	} 

	return new SSLSocket6(_ctx, handler, ssl, _rsock);
}

SSLContext * SSLServerSocket6::GetContext()
{
	return _ctx;
}

InetAddress * SSLServerSocket6::GetInetAddress()
{
	return _local;
}

int SSLServerSocket6::GetLocalPort()
{
	return ntohs(_lsock.sin6_port);
}

void SSLServerSocket6::Close()
{
	if (_is_closed == true) {
		return;
	}

	if (close(_fd) != 0) {
		throw SocketException("Unknow close exception");
	}

	_is_closed = true;
}

bool SSLServerSocket6::IsClosed()
{
	return _is_closed;
}

}
