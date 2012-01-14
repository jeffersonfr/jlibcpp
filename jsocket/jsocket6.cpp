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
#include "jsocket6.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jioexception.h"
#include "jinetaddress6.h"

namespace jsocket {

Socket6::Socket6(InetAddress *addr_, int port_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jsocket::Socket");
	
	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	CreateSocket();
	ConnectSocket(addr_, port_);
	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

Socket6::Socket6(InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jsocket::Socket");

	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	CreateSocket();
	BindSocket(local_addr_, local_port_);
	ConnectSocket(addr_, port_);
	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

Socket6::Socket6(std::string host_, int port_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jsocket::Socket");

	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	InetAddress *address = InetAddress6::GetByName(host_);

	CreateSocket();
	ConnectSocket(address, port_);
	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

Socket6::Socket6(std::string host_, int port_, InetAddress *local_addr_, int local_port_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jsocket::Socket");

	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	InetAddress *address = InetAddress6::GetByName(host_);

	CreateSocket();
	BindSocket(local_addr_, local_port_);
	ConnectSocket(address, port_);
	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

Socket6::~Socket6()
{
	try {
		Close();
	} catch (...) {
	}

	if (_is != NULL) {
		delete _is;
		_is = NULL;
	}

	if (_os != NULL) {
		delete _os;
		_os = NULL;
	}

	if (_address != NULL) {
		delete _address;
		_address = NULL;
	}
}

/** Private */

Socket6::Socket6(jsocket_t handler_, struct sockaddr_in6 server_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jsocket::Socket");

	char straddr[INET6_ADDRSTRLEN];

	_lsock.sin6_family = AF_INET6;
	_lsock.sin6_flowinfo = 0;
	_lsock.sin6_scope_id = 0;
	_lsock.sin6_addr = in6addr_any;
	_lsock.sin6_port = htons(0);
	
	_fd = handler_;
	_server_sock = server_;

	_address = InetAddress6::GetByName(std::string(inet_ntop(AF_INET6, &(_lsock.sin6_addr), straddr, sizeof(straddr))));

	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

void Socket6::CreateSocket()
{
#ifdef _WIN32
	if ((_fd = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
#else
	if ((_fd = ::socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0) {
#endif
		throw SocketException("Socket creation exception");
	}

	_is_closed = false;
}

void Socket6::BindSocket(InetAddress *local_addr_, int local_port_)
{
	memset(&_lsock, 0, sizeof(_lsock));

	_lsock.sin6_family = AF_INET6;
	_lsock.sin6_flowinfo = 0;
	_lsock.sin6_scope_id = 0;

	if (local_addr_ == NULL) {
		_lsock.sin6_addr = in6addr_any;
	} else {
		inet_pton(AF_INET6, local_addr_->GetHostAddress().c_str(), &(_lsock.sin6_addr));
	}

	_lsock.sin6_port = htons(local_port_);

	if (bind(_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) < 0) {
		throw SocketException("Socket bind exception");
	}
}

void Socket6::ConnectSocket(InetAddress *addr_, int port_)
{
	_address = addr_;

	memset(&_server_sock, 0, sizeof(_server_sock));

	_lsock.sin6_family = AF_INET6;
	_lsock.sin6_flowinfo = 0;
	_lsock.sin6_scope_id = 0;

	inet_pton(AF_INET6, _address->GetHostAddress().c_str(), &(_server_sock.sin6_addr));

	_server_sock.sin6_port = htons(port_);

	int r;

#ifdef _WIN32
	if (_timeout > 0) {
		u_long opt = 1;

		if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
			throw SocketException("Invalid connection parameters exception");
		}

		r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));

		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			throw SocketException("Socket connection exception");
		}

		if (r != 0) {
			fd_set wset;
			struct timeval t;

			t.tv_sec = _timeout/1000;
			t.tv_usec = (_timeout%1000)*1000;

			FD_ZERO(&wset);
			FD_SET(_fd, &wset);

			r = select(_fd + 1, &wset, &wset, &wset, &t);

			if (r <= 0) {
				opt = 0;

				if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
					throw SocketException("Invalid connection parameters exception");
				}

				shutdown(_fd, 2);

				if (r == 0) {
					throw SocketException("Socket connection timeout exception");
				} else if (r < 0) {
					throw SocketException("Socket connection exception");
				}
			}

			int optlen = sizeof(r);

			getsockopt(_fd, SOL_SOCKET, SO_ERROR, (char *)&r, &optlen);

			if (r != 0) {
				throw SocketException("Unknown socket exception");
			}
		}

		opt = 0;

		if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
			throw SocketException("Socket connection exception");
		}
	} else {
		r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
	}
#else
	if (_timeout > 0) {
		int opt = 1;

		ioctl(_fd, FIONBIO, &opt);

		r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));

		if (errno != EINPROGRESS) {
			throw SocketException("Socket connection exception");
		}

		if (r != 0) {
			fd_set wset;
			struct timeval t;

			t.tv_sec = _timeout/1000;
			t.tv_usec = (_timeout%1000)*1000;

			FD_ZERO(&wset);
			FD_SET(_fd, &wset);

			r = select(_fd + 1, &wset, &wset, &wset, &t);

			if (r <= 0) {
				opt = 0;

				if (ioctl(_fd, FIONBIO, &opt) < 0) {
					throw SocketException("Socket connection exception");
				}

				shutdown(_fd, SHUT_RDWR);

				if (r == 0) {
					throw SocketException("Socket connection timeout exception");
				} else if (r < 0) {
					throw SocketException("Socket connection exception");
				}
			}

			int optlen = sizeof(r);

			getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void *)&r, (socklen_t *)&optlen);

			if (r != 0) {
				throw SocketException("Unknown socket exception");
			}
		}

		opt = 0;

		if (ioctl(_fd, FIONBIO, &opt) < 0) {
			throw SocketException("Socket connection exception");
		}
	} else {
		r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
	}
#endif

#ifdef _WIN32
	if (r == SOCKET_ERROR) {
#else
	if (r < 0) {
#endif
		throw SocketException("Socket connection exception");
	}
}

void Socket6::InitStreams(int64_t rbuf_, int64_t wbuf_)
{
	_is = new SocketInputStream((Connection *)this, &_is_closed, rbuf_);
	_os = new SocketOutputStream((Connection *)this, &_is_closed, wbuf_);
}

/** End */

jsocket_t Socket6::GetHandler()
{
	return _fd;
}

int Socket6::Send(const char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

#ifdef _WIN32
	return Socket6::Send(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fd;
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Invalid send parameters exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket send timeout exception");
	} else {
		if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLWRBAND)) {
			return Socket6::Send(data_, size_);
		}
	}
#endif

	return -1;
}

int Socket6::Send(const char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

	int flags;

	if (block_ == true) {
#ifdef _WIN32
		flags = 0;
#else
		flags = MSG_NOSIGNAL;
#endif
	} else {
#ifdef _WIN32
		flags = 0;
#else
		flags = MSG_NOSIGNAL | MSG_DONTWAIT;
#endif
	}

	int n = ::send(_fd, data_, size_, flags);

#ifdef _WIN32
	if (n == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAECONNABORTED) {
			throw SocketTimeoutException("Socket send timeout exception");
		} else {
			throw SocketTimeoutException("Socket send exception");
		}
	}
#else
	if (n < 0) {
		if (errno == EAGAIN) {
			if (block_ == true) {
				throw SocketTimeoutException("Socket send timeout exception");
			} else {
				// INFO:: non-blocking socket, no data read
				n = 0;
			}
		} else if (errno == EPIPE || errno == ECONNRESET) {
			Close();

			throw SocketException("Broken pipe exception");
		} else {
			throw SocketTimeoutException("Socket send exception");
		}
	}
#endif

	_sent_bytes += n;

	return n;
}

int Socket6::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

#ifdef _WIN32
	return Socket6::Receive(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fd;
	ufds[0].events = POLLIN | POLLRDBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Invalid receive parameters exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket read timeout exception");
	} else {
		if ((ufds[0].revents & POLLIN) || (ufds[0].revents & POLLRDBAND)) {
			return Socket6::Receive(data_, size_);
		}
	}
#endif

	return -1;
}

int Socket6::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

	int flags = 0;

	if (block_ == false) {
#ifdef _WIN32
		flags = 0;
#else
		flags = MSG_DONTWAIT;
#endif
	}

	int n = ::recv(_fd, data_, size_, flags);

#ifdef _WIN32
	if (n == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) {
			throw SocketTimeoutException("Socket receive timeout exception");
		} else {
			throw jio::IOException("Socket read exception");
		}
	} else if (n == 0) {
		Close();

		throw SocketException("Broken pipe exception");
	}
#else 
	if (n < 0) {
		if (errno == EAGAIN) {
			if (block_ == true) {
				throw SocketTimeoutException("Socket receive timeout exception");
			} else {
				// INFO:: non-blocking socket, no data read
				n = 0;
			}
		} else {
			throw jio::IOException("Socket read exception");
		}
	} else if (n == 0) {
		Close();

		throw jio::IOException("Peer has shutdown");
	}
#endif

	_receive_bytes += n;

	return n;
}

void Socket6::Close()
{
	if (_is_closed == true) {
		return;
	}

#ifdef _WIN32
	if (closesocket(_fd) < 0) {
#else
	if (close(_fd) != 0) {
#endif
		throw SocketException("Unknown close exception");
	}

	_is_closed = true;
}

jio::InputStream * Socket6::GetInputStream()
{
	return (jio::InputStream *)_is;
}

jio::OutputStream * Socket6::GetOutputStream()
{
	return (jio::OutputStream *)_os;
}

InetAddress * Socket6::GetInetAddress()
{
	return _address;
}

int Socket6::GetLocalPort()
{
	return ntohs(_lsock.sin6_port);
}

int Socket6::GetPort()
{
	return ntohs(_server_sock.sin6_port);
}

int64_t Socket6::GetSentBytes()
{
	return _sent_bytes + _os->GetSentBytes();
}

int64_t Socket6::GetReadedBytes()
{
	return _receive_bytes + _is->GetReadedBytes();
}

SocketOptions * Socket6::GetSocketOptions()
{
	return new SocketOptions(_fd, JCT_TCP);
}

std::string Socket6::what()
{
	char *port = (char *)malloc(10);

	sprintf(port, "%u", GetPort());

	return GetInetAddress()->GetHostName() + ":" + port;
}

}
