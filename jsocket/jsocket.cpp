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
#include "jsocket.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jsocketstreamexception.h"

#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/ioctl.h>
#include <poll.h>
#endif

#include <errno.h>
#include <strings.h>

namespace jsocket {

Socket::Socket(InetAddress *addr_, int port_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
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

Socket::Socket(InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
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

Socket::Socket(std::string host_, int port_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::Socket");
	
	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	InetAddress *address = InetAddress::GetByName(host_);

	CreateSocket();
	ConnectSocket(address, port_);
	InitStreams(rbuf_, wbuf_);
	
	_is_closed = false;
}

Socket::Socket(std::string host_, int port_, InetAddress *local_addr_, int local_port_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::Socket");
	
	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	InetAddress *address = InetAddress::GetByName(host_);
  
	CreateSocket();
	BindSocket(local_addr_, local_port_);
	ConnectSocket(address, port_);
	InitStreams(rbuf_, wbuf_);
	
	_is_closed = false;
}

Socket::~Socket()
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

Socket::Socket(int handler_, sockaddr_in server_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::Socket");
	
#ifdef _WIN32
	// int len;
    
	_lsock.sin_family = AF_INET;
#else
	// socklen_t len;
    
	_lsock.sin_family = AF_INET;
#endif
    
	/* CHANGE:: este codigo estah gerando erro no DEBIAN
	if (getpeername(handler_, (struct sockaddr *)&_lsock, &len) < 0) {
		throw SocketException("Connetion error");
	}
        
	if (getsockname(handler_, (struct sockaddr *)&_lsock, &len) < 0) {
		throw SocketException("Connection error");
	}
	*/
    
	_fd = handler_;
	_server_sock = server_;
    
	_address = InetAddress::GetByName((std::string)inet_ntoa(server_.sin_addr));
	
	InitStreams(rbuf_, wbuf_);
	
	_is_closed = false;
}

void Socket::CreateSocket()
{
   _fd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

#ifdef _WIN32
   if (_fd == INVALID_SOCKET) {
#else
	if (_fd < 0) {
#endif
		throw SocketException("Create socket error");
    }
}

void Socket::BindSocket(InetAddress *local_addr_, int local_port_)
{
	memset(&_lsock, 0, sizeof(_lsock));

	_lsock.sin_family = AF_INET;

	if (local_addr_ == NULL) {
		_lsock.sin_addr.s_addr = INADDR_ANY;
	} else {
		_lsock.sin_addr.s_addr = inet_addr(local_addr_->GetHostAddress().c_str());
	}
    
	_lsock.sin_port = htons(local_port_);

	if (bind(_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) < 0) {
		throw SocketException("Bind socket error");
	}
}

void Socket::ConnectSocket(InetAddress *addr_, int port_)
{
	_address = addr_;
   
	memset(&_server_sock, 0, sizeof(_server_sock));
   
	_server_sock.sin_family = AF_INET;
	_server_sock.sin_addr.s_addr  = inet_addr(addr_->GetHostAddress().c_str());
	_server_sock.sin_port = htons(port_);

	int r;
	
#ifdef _WIN32
	if (_timeout > 0) {
		u_long opt = 1;

		if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
			throw SocketException("Socket non-blocking error");
		}

		r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
		
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			opt = 0;

			if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
				throw SocketException("Socket non-blocking error");
			}

			throw SocketException("Connect socket error");
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
					throw SocketException("Socket non-blocking error");
				}

				shutdown(_fd, 2);
				
				if (r == 0) {
					throw SocketException("Connect timeout error");
				} else if (r < 0) {
					throw SocketException("Connect socket error");
				}
			}
			
			int optlen = sizeof(r);

			getsockopt(_fd, SOL_SOCKET, SO_ERROR, (char *)&r, &optlen);

			if (r != 0) {
				throw SocketException("Can't connect socket");
			}
		}
		
		opt = 0;

		if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
			throw SocketException("Socket non-blocking error");
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
			opt = 0;

			if (ioctl(_fd, FIONBIO, &opt) < 0) {
				throw SocketException("Socket non-blocking error");
			}
			
			throw SocketException("Connect socket error");
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
					throw SocketException("Socket non-blocking error");
				}

				shutdown(_fd, SHUT_RDWR);
				
				if (r == 0) {
					throw SocketException("Connect timeout error");
				} else if (r < 0) {
					throw SocketException("Connect socket error");
				}
			}
			
			int optlen = sizeof(r);

			getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void *)&r, (socklen_t *)&optlen);

			if (r != 0) {
				throw SocketException("Can't connect socket");
			}
		}
		
		opt = 0;

		if (ioctl(_fd, FIONBIO, &opt) < 0) {
			throw SocketException("Socket non-blocking error");
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
		throw SocketException("Connect socket error");
	}
}

void Socket::InitStreams(int rbuf_, int wbuf_)
{
	_is = new SocketInputStream((Connection *)this, &_is_closed, rbuf_);
	_os = new SocketOutputStream((Connection *)this, &_is_closed, wbuf_);
}

/** End */

#ifdef _WIN32
SOCKET Socket::GetHandler()
#else
int Socket::GetHandler()
#endif
{
	return _fd;
}

int Socket::Send(const char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
	}

#ifdef _WIN32
	return Socket::Send(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fd;
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Send timeout exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket send timeout exception");
	} else {
	    if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLWRBAND)) {
			return Socket::Send(data_, size_);
	    }
	}
#endif

	return -1;
}

int Socket::Send(const char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
	}
	
	int flags;

	if (block_ == true) {
#ifdef _WIN32
		flags = 0;
#elif _CYGWIN
		flags = 0;
#else
		flags = MSG_NOSIGNAL;
#endif
	} else {
#ifdef _WIN32
		flags = 0;
#elif _CYGWIN
		flags = 0;
#else
		flags = MSG_NOSIGNAL | MSG_DONTWAIT;
#endif
	}

	int n = ::send(_fd, data_, size_, flags);

#ifdef _WIN32
	if (n < 0) {
#else
	if (n < 0 && errno == EAGAIN) {
		if (block_ == false) {
			throw SocketStreamException("Socket buffer is empty");
		} else {
			throw SocketTimeoutException("Socket send timeout exception");
		}
	} else if (n < 0) {
#endif
		throw SocketStreamException("Send socket error");
	}
		
	_sent_bytes += n;
	
	return n;
}

int Socket::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
#ifdef _WIN32
	return Socket::Receive(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fd;
	ufds[0].events = POLLIN | POLLRDBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Receive timed exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket receive timeout exception");
	} else {
	    if ((ufds[0].revents & POLLIN) || (ufds[0].revents & POLLRDBAND)) {
			return Socket::Receive(data_, size_);
	    }
	}
#endif

	return -1;
}

int Socket::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	int flags;

	if (block_ == true) {
#ifdef _WIN32
		flags = 0;
#elif _CYGWIN
		flags = 0;
#else
		flags = 0;
#endif
	} else {
#ifdef _WIN32
		flags = 0;
#elif _CYGWIN
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
			throw SocketStreamException("Read socket error");
		}
	} else if (n == 0) {
#else 
	if (n < 0 && errno == EAGAIN) {
		if (block_ == false) {
			throw SocketStreamException("Socket buffer is empty");
		} else {
			throw SocketTimeoutException("Socket receive timeout exception");
		}
	} else if (n < 0) {
		throw SocketStreamException("Read socket error");
	} else if (n == 0) {
#endif
		//throw SocketException("Peer has shutdown");
		return -1;
	}

	_receive_bytes += n;
    
	return n;
}

void Socket::Close()
{
#ifdef _WIN32
	if (_is_closed == false) {
		_is_closed = true;

		if (closesocket(_fd) < 0) {
#else
	if (_is_closed == false) {
		_is_closed = true;

		if (close(_fd) != 0) {
#endif
			throw SocketException("Close socket error");
		}
	}
}

jio::InputStream * Socket::GetInputStream()
{
	return (jio::InputStream *)_is;
}

jio::OutputStream * Socket::GetOutputStream()
{
	return (jio::OutputStream *)_os;
}

InetAddress * Socket::GetInetAddress()
{
	return _address;
}

int Socket::GetLocalPort()
{
	return ntohs(_lsock.sin_port);
}

int Socket::GetPort()
{
	return ntohs(_server_sock.sin_port);
}

int64_t Socket::GetSentBytes()
{
	return _sent_bytes + _os->GetSentBytes();
}

int64_t Socket::GetReceiveBytes()
{
	return _receive_bytes + _is->GetReceiveBytes();
}

SocketOption * Socket::GetSocketOption()
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	return new SocketOption(_fd, TCP_SOCKET);
}

std::string Socket::what()
{
	char *port = (char *)malloc(10);

	sprintf(port, "%u", GetPort());

	return GetInetAddress()->GetHostName() + ":" + port;
}

}
