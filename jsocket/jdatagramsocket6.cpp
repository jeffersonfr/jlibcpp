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
#include "jdatagramsocket6.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jsocketstreamexception.h"
#include "junknownhostexception.h"
#include "jinetaddress6.h"

namespace jsocket {

int DatagramSocket6::_used_port = 1024;

DatagramSocket6::DatagramSocket6(std::string host_, int port_, bool stream_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_UDP)
{
	jcommon::Object::SetClassName("jsocket::DatagramSocket6");
	
	_stream = stream_;
	_address = NULL;
	_is = NULL;
	_os = NULL;
	_is_closed = true;
	_timeout = timeout_;

	CreateSocket();
	ConnectSocket(InetAddress6::GetByName(host_), port_);
	InitStream(rbuf_, wbuf_);

	_sent_bytes = 0;
	_receive_bytes = 0;
}

DatagramSocket6::DatagramSocket6(int port_, bool stream_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_UDP)
{
	jcommon::Object::SetClassName("jsocket::DatagramSocket6");

	_address = NULL;
	_is = NULL;
	_os = NULL;
	_is_closed = true;
	_stream = stream_;
	_timeout = timeout_;

	try {
		_address = InetAddress6::GetByName("localhost");
	} catch (UnknownHostException &) {
		// WARN:: verify if GetInetAddress6() == NULL
		_address = NULL;
	}

	CreateSocket();

	if (port_ == 0) {
		while(true) {
			try {
				BindSocket(_address, ++_used_port);
			} catch(SocketException &) {
				continue;
			}

			break;
		}
	} else {
		BindSocket(_address, port_);
	}

	InitStream(rbuf_, wbuf_);

	_sent_bytes = 0;
	_receive_bytes = 0;
}

DatagramSocket6::~DatagramSocket6()
{
	try {
		Close();
	} catch (...) {
	}

	if ((void *)_address != NULL) {
		delete _address;
	}

	if ((void *)_is != NULL) {
		delete _is;
	}

	if ((void *)_os != NULL) {
		delete _os;
	}
}

/** Private */

void DatagramSocket6::CreateSocket()
{
#ifdef _WIN32
	if ((_fd = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
#else
	if ((_fd = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
#endif
		throw SocketException("Create datagram socket error");
	}

	_is_closed = false;
}

void DatagramSocket6::BindSocket(InetAddress *addr_, int local_port_)
{
	int opt = 1;

	memset(&_lsock, 0, sizeof(_lsock));
   
	_lsock.sin6_family = AF_INET6;
	_lsock.sin6_flowinfo = 0;
	_lsock.sin6_addr = in6addr_any;
	_lsock.sin6_scope_id = 0;
   
	if(local_port_ > 0) {
		_lsock.sin6_port = htons(local_port_);
	} else {
		_lsock.sin6_port = htons(-1);
	}

#ifdef _WIN32
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
#endif
    
#ifdef _WIN32
   if (::bind (_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) == SOCKET_ERROR) {
#else
	if (::bind(_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) < 0) {
#endif
		throw SocketException("Bind datagram socket error");
	}
}

void DatagramSocket6::ConnectSocket(InetAddress *addr_, int port_)
{
	_address = addr_;
	
	memset(&_server_sock, 0, sizeof(_server_sock));
	
	_lsock.sin6_family = AF_INET6;
	_lsock.sin6_flowinfo = 0;
	_lsock.sin6_scope_id = 0;
	_lsock.sin6_port = htons(port_);
	
	if(_address == NULL) {
		_lsock.sin6_addr = in6addr_any;
	} else {
		inet_pton(AF_INET6, _address->GetHostAddress().c_str(), &(_lsock.sin6_addr));
	}

	int r;
	
	if (_stream == true) {
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

		if (ioctl(_fd, FIONBIO, &opt) < 0) {
			throw SocketException("Socket non-blocking error");
		}

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
			throw SocketException("Connect udp socket error");
		}
	}
}

void DatagramSocket6::InitStream(int rbuf_, int wbuf_)
{
	if (_stream == false) {
		_is = new SocketInputStream((Connection *)this, &_is_closed, (struct sockaddr *)&_server_sock, rbuf_);
		_os = new SocketOutputStream((Connection *)this, &_is_closed, (struct sockaddr *)&_server_sock, wbuf_);
	} else {
		_is = new SocketInputStream((Connection *)this, &_is_closed, rbuf_);
		_os = new SocketOutputStream((Connection *)this, &_is_closed, wbuf_);
	}
}

/** End */

jsocket_t DatagramSocket6::GetHandler()
{
	return _fd;
}

jio::InputStream * DatagramSocket6::GetInputStream()
{
	return (jio::InputStream *)_is;
}

jio::OutputStream * DatagramSocket6::GetOutputStream()
{
	return (jio::OutputStream *)_os;
}

int DatagramSocket6::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
#ifdef _WIN32
	return DatagramSocket6::Receive(data_, size_);
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
			return DatagramSocket6::Receive(data_, size_, true);
	    }
	}
#endif

	return -1;
}

int DatagramSocket6::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	int n,
		flags,
		length = sizeof(_server_sock);

	if (block_ == true) {
		// CHANGE:: call SocketOptions

#ifdef _WIN32
		flags = 0;
#else
		flags = 0;
#endif
	} else {
#ifdef _WIN32
		flags = 0;
#else
		flags = MSG_DONTWAIT;
#endif
	}

#ifdef _WIN32
	n = ::recvfrom(_fd, data_, size_, flags, (struct sockaddr *)&_server_sock, &length);

	if (n == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) {
			throw SocketTimeoutException("Socket receive timeout exception");
		} else {
			throw SocketStreamException("Read socket error");
		}
	} else if (n == 0) {
		throw SocketException("Connection closed");
	}
#else
	n = ::recvfrom(_fd, data_, size_, flags, (struct sockaddr *)&_server_sock, (socklen_t *)&length);
	
	if (n < 0) {
	   if (errno == EAGAIN) {
			if (block_ == false) {
				throw SocketStreamException("Socket buffer is empty");
			} else {
				throw SocketTimeoutException("Socket receive timeout exception");
			}
		} else {
			throw SocketStreamException("Read socket error");
		}
	}
#endif

	_receive_bytes += n;

    return n;
}

int DatagramSocket6::Send(const char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
	}
	
#ifdef _WIN32
	return DatagramSocket6::Send(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fd;
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Send timed exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket send timeout exception");
	} else {
	    if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLWRBAND)) {
			return DatagramSocket6::Send(data_, size_);
	    }
	}
#endif

	return -1;
}

int DatagramSocket6::Send(const char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
	}
	
	int n,
	   	flags = 0;

	if (block_ == false) {
#ifdef _WIN32
		flags = 0;
#else
		flags = MSG_NOSIGNAL | MSG_DONTWAIT;
#endif
	}

	if (_stream == true) {	
		n = ::send(_fd, data_, size_, flags);
	} else {
		n = ::sendto(_fd, data_, size_, flags, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
	}

#ifdef _WIN32
	if (n == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAECONNABORTED) {
			throw SocketTimeoutException("Socket send timeout exception");
		} else {
			throw SocketStreamException("Send udp data error");
		}
	}
#else
	if (n < 0) {
	    if (errno == EAGAIN) {
			if (block_ == false) {
				throw SocketStreamException("Socket buffer is empty");
			} else {
				throw SocketTimeoutException("Socket send timeout exception");
			}
		} else {
			throw SocketStreamException("Send udp data error");
		}
	}
#endif

	_sent_bytes += n;
	
	return n;
}

void DatagramSocket6::Close()
{
	if (_is_closed == true) {
		return;
	}

#ifdef _WIN32
	if (closesocket(_fd) < 0) {
#else
	if (close(_fd) != 0) {
#endif
		throw SocketException("Close socket error");
	}

	_is_closed = true;
}

InetAddress * DatagramSocket6::GetInetAddress()
{
	return _address;
}

int DatagramSocket6::GetLocalPort()
{
	return ntohs(_lsock.sin6_port);
}

int DatagramSocket6::GetPort()
{
	return ntohs(_server_sock.sin6_port);
}

int64_t DatagramSocket6::GetSentBytes()
{
	return _sent_bytes + _os->GetSentBytes();
}

int64_t DatagramSocket6::GetReadedBytes()
{
	return _receive_bytes + _is->GetReadedBytes();
}

SocketOption * DatagramSocket6::GetSocketOption()
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}

	return new SocketOption(_fd, JCT_UDP);
}

std::string DatagramSocket6::what()
{
	char port[20];
   
	sprintf(port, "%u", GetPort());

	return GetInetAddress()->GetHostName() + ":" + port;
}

}
