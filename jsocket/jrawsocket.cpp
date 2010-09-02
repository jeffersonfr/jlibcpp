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
#include "jrawsocket.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jsocketstreamexception.h"

namespace jsocket {

int RawSocket::_used_port = 1024;

RawSocket::RawSocket(std::string device_, bool promisc_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(RAW_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::RawSocket");
	
	_device = device_;
	_promisc = promisc_;
	_address = NULL;
	_is = NULL;
	_os = NULL;
	_is_closed = true;
	_timeout = timeout_;

	CreateSocket();
	BindSocket();
	InitStream(rbuf_, wbuf_);

	_sent_bytes = 0;
	_receive_bytes = 0;

	_is_closed = false;
}

RawSocket::~RawSocket()
{
	ioctl(_fd, SIOCSIFFLAGS, &(_ifr));

	try {
		Close();
	} catch (...) {
	}

	/*
		 if (_address) {
		 delete _address;
		 }

		 if (_is) {
		 delete _is;
		 }

		 if (_os) {
		 delete _os;
		 }
		 */
}

/** Private */

void RawSocket::CreateSocket()
{
#ifdef _WIN32
	_fd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);

	if (_fd == INVALID_SOCKET) {
		throw SocketException("Create socket raw error");
	}
#else
	_fd = ::socket(PF_PACKET, SOCK_RAW, (_promisc == true)?ETH_P_ALL:ETH_P_IP);

	if (_fd < 0) {
		throw SocketException("Create socket raw error");
	}

	struct ifreq ifr;

	memset(&_ifr, 0, sizeof(_ifr));
	strncpy(_ifr.ifr_name, _device.c_str(), sizeof(_ifr.ifr_name));

	if (ioctl(_fd, SIOCGIFFLAGS, &_ifr)<0) {
		throw SocketException("Net device failed");
	}

	if (_promisc) {
		ifr.ifr_flags |= IFF_PROMISC;
	} else {
		ifr.ifr_flags &= ~IFF_PROMISC;
	}

	if (ioctl(_fd, SIOCSIFFLAGS, &_ifr) < 0) {
		throw SocketException("Net device cannot suport promisc mode");
	}

	if (ioctl(_fd, SIOCGIFINDEX, &_ifr) < 0) {
		throw SocketException("Net device access failed");
	}

	_index_device = _ifr.ifr_ifindex;

	if (_index_device < 0) {
		throw SocketException("Net device doesn't exists");
	}
#endif
}

void RawSocket::BindSocket()
{
	struct sockaddr_ll sock_ether;

	memset(&sock_ether, 0, sizeof(sock_ether));
	sock_ether.sll_family = AF_PACKET;
	sock_ether.sll_protocol = htons((_promisc == true)?ETH_P_ALL:ETH_P_IP);
	sock_ether.sll_ifindex = _index_device;
	sock_ether.sll_pkttype = (_promisc == true)?PACKET_OTHERHOST:PACKET_HOST;

	if (bind(_fd, (struct sockaddr *)(&sock_ether), sizeof(sock_ether)) < 0) {
		throw SocketException("Bind raw socket error");
	}
}

void RawSocket::InitStream(int rbuf_, int wbuf_)
{
	_is = new SocketInputStream((Connection *)this, &_is_closed, RemoteAddress(), rbuf_);
	_os = new SocketOutputStream((Connection *)this, &_is_closed, RemoteAddress(), wbuf_);
}

/** End */

#ifdef _WIN32
SOCKET RawSocket::GetHandler()
#else
int RawSocket::GetHandler()
#endif
{
	return _fd;
}

jio::InputStream * RawSocket::GetInputStream()
{
	return (jio::InputStream *)_is;
}

jio::OutputStream * RawSocket::GetOutputStream()
{
	return (jio::OutputStream *)_os;
}

int RawSocket::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}

#ifdef _WIN32
	return RawSocket::Receive(data_, size_);
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
			return RawSocket::Receive(data_, size_);
		}
	}
#endif

	return -1;
}

int RawSocket::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}

	int n;

#ifdef _WIN32
	/*
		 n = ::recvfrom(_fd, data_, size_, 0, (struct sockaddr *)&_server_sock, &length);

		 if (n == SOCKET_ERROR) {
		 if (WSAGetLastError() == WSAETIMEDOUT) {
		 throw SocketTimeoutException("Socket receive timeout exception");
		 } else {
		 throw SocketStreamException("Read socket error");
		 }
		 } else if (n == 0) {
		 throw SocketException("Connection closed");
		 }
		 */
#else
	// n = ::recvfrom(_fd, data_, size_, 0, (struct sockaddr *)&_lsock, (socklen_t *)&length);
	n = ::read(_fd, data_, size_);

	if (n < 0) {
		if (errno == EAGAIN) {
			throw SocketTimeoutException("Socket receive timeout exception");
		} else {
			throw SocketStreamException("Read socket error");
		}
	}
#endif

	_receive_bytes += n;

	return n;
}

int RawSocket::Send(const char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
	}

#ifdef _WIN32
	return RawSocket::Send(data_, size_);
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
			return RawSocket::Send(data_, size_);
		}
	}
#endif

	return -1;
}

int RawSocket::Send(const char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
	}

	int n;

#ifdef _WIN32
	n = ::sendto(_fd, data_, size_, 0, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
#else
	n = ::write(_fd, data_, size_);
#endif

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
			throw SocketTimeoutException("Socket send timeout exception");
		} else {
			throw SocketStreamException("Send udp data error");
		}
	}
#endif

	_sent_bytes += n;

	return n;
}

void RawSocket::Close()
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

sockaddr_in RawSocket::LocalAddress()
{
	return _lsock;
}

sockaddr_in RawSocket::RemoteAddress()
{
	return _server_sock;
}

InetAddress * RawSocket::GetInetAddress()
{
	return _address;
}

int RawSocket::GetLocalPort()
{
	return ntohs(_lsock.sin_port);
}

int RawSocket::GetPort()
{
	return ntohs(_server_sock.sin_port);
}

int64_t RawSocket::GetSentBytes()
{
	return _sent_bytes + _os->GetSentBytes();
}

int64_t RawSocket::GetReceiveBytes()
{
	return _receive_bytes + _is->GetReceiveBytes();
}

SocketOption * RawSocket::GetSocketOption()
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}

	return new SocketOption(_fd, RAW_SOCKET);
}

unsigned short RawSocket::Checksum(unsigned short *addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register int sum = 0;
	u_short answer = 0;
	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}
	if (nleft == 1) {
		*(u_char *)(&answer) = *(u_char *) w;
		sum += answer;
	}
	sum = (sum >> 16) + (sum & 0xF0F0);
	sum += (sum >> 16);
	answer = ~sum;
	return(answer);
}

std::string RawSocket::what()
{
	char port[20];

	sprintf(port, "%u", GetPort());

	return GetInetAddress()->GetHostName() + ":" + port;
}

}
