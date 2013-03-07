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
#include "jioexception.h"

namespace jsocket {

int RawSocket::_used_port = 1024;

RawSocket::RawSocket(std::string device_, bool promisc_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_RAW)
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
}

RawSocket::~RawSocket()
{
#ifdef _WIN32
#else
	ioctl(_fd, SIOCSIFFLAGS, &(_ifr));
#endif

	try {
		Close();
	} catch (...) {
	}
}

/** Private */

void RawSocket::CreateSocket()
{
#ifdef _WIN32
	if ((_fd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) == INVALID_SOCKET) {
		throw SocketException("Socket handling error");
	}
#else
	if ((_fd = ::socket(PF_PACKET, SOCK_RAW, (_promisc == true)?ETH_P_ALL:ETH_P_IP)) < 0) {
		throw SocketException("Socket handling error");
	}

	_is_closed = false;

	struct ifreq ifr;

	memset(&_ifr, 0, sizeof(_ifr));
	strncpy(_ifr.ifr_name, _device.c_str(), sizeof(_ifr.ifr_name));

	if (ioctl(_fd, SIOCGIFFLAGS, &_ifr)<0) {
		throw SocketException("Cannot access network interface flags");
	}

	if (_promisc) {
		ifr.ifr_flags |= IFF_PROMISC;
	} else {
		ifr.ifr_flags &= ~IFF_PROMISC;
	}

	if (ioctl(_fd, SIOCSIFFLAGS, &_ifr) < 0) {
		throw SocketException("Cannot put network interface in promiscuous mode");
	}

	if (ioctl(_fd, SIOCGIFINDEX, &_ifr) < 0) {
		throw SocketException("Cannot access network interface index");
	}

	_index_device = _ifr.ifr_ifindex;

	if (_index_device < 0) {
		throw SocketException("Network interface do not exists");
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
		throw SocketException("Binding error");
	}
}

void RawSocket::InitStream(int rbuf_, int wbuf_)
{
	_is = new SocketInputStream((Connection *)this, (struct sockaddr *)&_server_sock, rbuf_);
	_os = new SocketOutputStream((Connection *)this, (struct sockaddr *)&_server_sock, wbuf_);
}

/** End */

jsocket_t RawSocket::GetHandler()
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
		throw SocketException("Connection closed exception");
	}

#ifdef _WIN32
	return RawSocket::Receive(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fd;
	ufds[0].events = POLLIN | POLLRDBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Invalid receive parameters exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket input timeout error");
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
		throw SocketException("Connection closed exception");
	}

	int n;

#ifdef _WIN32
	/*
	n = ::recvfrom(_fd, data_, size_, 0, (struct sockaddr *)&_server_sock, &length);

	if (n == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) {
			throw SocketTimeoutException("Socket input timeout error");
		} else {
			throw jio::IOException("Socket input error");
		}
	} else if (n == 0) {
		throw jio::IOException("Peer shutdown exception");
	}
	*/
#else
	// n = ::recvfrom(_fd, data_, size_, 0, (struct sockaddr *)&_lsock, (socklen_t *)&length);
	n = ::read(_fd, data_, size_);

	if (n < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (block_ == true) {
				throw SocketTimeoutException("Socket input timeout error");
			}
		}
			
		throw jio::IOException("Socket input error");
	} else if (n == 0) {
		if (block_ == true) {
			Close();

			throw jio::IOException("Peer has shutdown");
		}
	}

	/*
	if (n < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (block_ == true) {
				throw SocketTimeoutException("Socket input timeout error");
			} else {
				// INFO:: non-blocking socket, no data read
				n = 0;
			}
		} else {
			throw jio::IOException("Socket input error");
		}
	} else if (n == 0) {
		Close();

		throw jio::IOException("Peer shutdown exception");
	}
	*/
#endif

	_receive_bytes += n;

	return n;
}

int RawSocket::Send(const char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

#ifdef _WIN32
	return RawSocket::Send(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fd;
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Invalid send parameters exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket output timeout error");
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
		throw SocketException("Connection closed exception");
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
			throw SocketTimeoutException("Socket output timeout error");
		} else {
			throw SocketTimeoutException("Socket output timeout error");
		}
	}
#else
	if (n < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (block_ == true) {
				throw SocketTimeoutException("Socket output timeout error");
			}
				
			throw SocketException("Socket output exception");
			
			/*
			if (block_ == true) {
				throw SocketTimeoutException("Socket output timeout error");
			} else {
				// INFO:: non-blocking socket, no data read
				n = 0;
			}
			*/
		} else if (errno == EPIPE || errno == ECONNRESET) {
			Close();

			throw SocketException("Broken pipe exception");
		} else {
			throw SocketTimeoutException("Socket output timeout error");
		}
	}
#endif

	_sent_bytes += n;

	return n;
}

void RawSocket::Close()
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

int64_t RawSocket::GetReadedBytes()
{
	return _receive_bytes + _is->GetReadedBytes();
}

SocketOptions * RawSocket::GetSocketOptions()
{
	return new SocketOptions(_fd, JCT_RAW);
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
