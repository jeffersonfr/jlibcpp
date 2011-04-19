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
#include "jmulticastsocket.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jsocketstreamexception.h"

namespace jsocket {

int MulticastSocket::_used_port = 1024;

MulticastSocket::MulticastSocket(std::string host_, int port_, int rbuf_, int wbuf_):
	jsocket::Connection(MCAST_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::MulticastSocket");
	
	_is = NULL;
	_os = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;

	CreateSocket();
	ConnectSocket(InetAddress::GetByName(host_), port_);
	BindSocket(InetAddress::GetByName(host_), port_);
	Join(InetAddress::GetByName(host_));
	InitStream(rbuf_, wbuf_);

	_is_closed = true;
}

MulticastSocket::~MulticastSocket()
{
	try {
		Close();
	} catch (...) {
	}

	if (_is != NULL) {
		delete _is;
	}

	if (_os != NULL) {
		delete _os;
	}
}

/** Private */

void MulticastSocket::CreateSocket()
{
	_fds = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_IP); // IPPROTO_MTP
	
	if (_fds < 0) {
		throw SocketException("Create multicast socket error");
	}

	_fdr = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_IP); // IPPROTO_MTP
	
	if (_fdr < 0) {
		throw SocketException("Create multicast socket error");
	}
}

void MulticastSocket::BindSocket(InetAddress *addr_, int local_port_)
{
	if (bind(_fdr, (struct sockaddr *)&_sock_r, sizeof(_sock_r)) < 0) {
		throw SocketException("Bind multicast socket error");
	}
}

void MulticastSocket::ConnectSocket(InetAddress *addr_, int port_)
{
	// Receive
	memset(&_sock_r, 0, sizeof(_sock_r));

	_sock_r.sin_family = AF_INET;
	_sock_r.sin_port = htons(port_);

#ifdef SOLARIS
	_sock_r.sin_addr.s_addr = htonl(INADDR_ANY);
#else
	if (addr_ == NULL) {
		_sock_r.sin_addr.s_addr = inet_addr(INADDR_ANY);
	} else {
		_sock_r.sin_addr.s_addr = inet_addr(addr_->GetHostAddress().c_str());
	}
#endif

	// Send
	memset(&_sock_s, 0, sizeof(_sock_s));

	_sock_s.sin_family = AF_INET;
	_sock_s.sin_port = htons(port_);
	_sock_s.sin_addr.s_addr = htonl(INADDR_ANY);
}

void MulticastSocket::InitStream(int rbuf_, int wbuf_)
{
	_is = new SocketInputStream((Connection *)this, &_is_closed, (struct sockaddr *)&_sock_r, rbuf_);
	_os = new SocketOutputStream((Connection *)this, &_is_closed, (struct sockaddr *)&_sock_s, wbuf_);
}

/** End */

#ifdef _WIN32
SOCKET MulticastSocket::GetHandler()
#else
int MulticastSocket::GetHandler()
#endif
{
	return _fdr;
}

jio::InputStream * MulticastSocket::GetInputStream()
{
	return (jio::InputStream *)_is;
}

jio::OutputStream * MulticastSocket::GetOutputStream()
{
	return (jio::OutputStream *)_os;
}

int MulticastSocket::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
#ifdef _WIN32
	return MulticastSocket::Receive(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fdr;
	ufds[0].events = POLLIN | POLLRDBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Receive timed exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket receive timeout exception");
	} else {
		if ((ufds[0].revents & POLLIN) || (ufds[0].revents & POLLRDBAND)) {
			return MulticastSocket::Receive(data_, size_);
		}
	}
#endif

	return -1;
}

int MulticastSocket::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}

	int n,
			flags = 0,
			length = sizeof(_sock_r);

	if (block_ == false) {
#ifdef _WIN32
		flags = 0;
#else
		flags = MSG_DONTWAIT;
#endif
	}


#ifdef _WIN32
	n = ::recvfrom(_fdr, data_, size_, flags, (struct sockaddr *)&_sock_r, &length);
#else
	n = ::recvfrom(_fdr, data_, size_, flags, (struct sockaddr *)&_sock_r, (socklen_t *)&length);
#endif
	
	if (n < 0 && errno == EAGAIN) {
		if (block_ == false) {
			throw SocketStreamException("Socket buffer is empty");
		} else {
			throw SocketTimeoutException("Socket receive timeout exception");
		}
	} else if (n < 0) {
		throw SocketStreamException("Read socket error");
	}

	_receive_bytes += n;

	return n;
}

int MulticastSocket::Send(const char *data, int size, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
	}

#ifdef _WIN32
	return MulticastSocket::Send(data, size);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fds;
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Send timeout exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket send timeout exception");
	} else {
		if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLWRBAND)) {
			return MulticastSocket::Send(data, size);
		}
	}
#endif

	return -1;
}

int MulticastSocket::Send(const char *data, int size, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
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

	int n = ::sendto(_fds, data, size, flags, (struct sockaddr *)&_sock_s, sizeof(_sock_s));

#ifdef _WIN32
	if (n == SOCKET_ERROR) {
		throw SocketException("Send udp data error");
	}
#else
	if (n < 0 && errno == EAGAIN) {
		if (block_ == false) {
			throw SocketStreamException("Socket buffer is empty");
		} else {
			throw SocketTimeoutException("Socket send timeout exception");
		}
	} else if (n < 0) {
		throw SocketException("Send udp data error");
	}
#endif

	_sent_bytes += n;
	
	return n;
}

void MulticastSocket::Join(std::string group_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	struct ip_mreq imr;

	imr.imr_multiaddr.s_addr = inet_addr(group_.c_str());
	imr.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef _WIN32
	if (setsockopt(_fdr, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&imr, sizeof(imr)) < 0) {
#else
	if (setsockopt(_fdr, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr)) < 0) {
#endif
		throw SocketException("Join group error");
	}

	_groups.push_back(group_);
}

void MulticastSocket::Join(InetAddress *group_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	struct ip_mreq imr;

	imr.imr_multiaddr.s_addr = inet_addr(group_->GetHostAddress().c_str());
	imr.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef _WIN32
	if (setsockopt(_fdr, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&imr, sizeof(imr)) < 0) {
#else
	if (setsockopt(_fdr, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr)) < 0) {
#endif
		throw SocketException("Join group error");
	}

	_groups.push_back(group_->GetHostAddress());
}

void MulticastSocket::Leave(std::string group_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	struct ip_mreq imr;

	for (std::vector<std::string>::iterator i=_groups.begin(); i!=_groups.end(); i++) {
		if (group_ == (*i)) {
			imr.imr_multiaddr.s_addr = inet_addr(group_.c_str());
			imr.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef _WIN32
			if (setsockopt(_fdr, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char *)&imr, sizeof(imr)) < 0) {
#else
			if (setsockopt(_fdr, IPPROTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(imr)) < 0) {
#endif
				throw SocketException("Leave group error");
			}

			// _groups.remove(*i);

			break;
		}
	}
}

void MulticastSocket::Leave(InetAddress *group_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	struct ip_mreq imr;
	std::string s = group_->GetHostAddress();

	for (std::vector<std::string>::iterator i=_groups.begin(); i!=_groups.end(); i++) {
		if (s == (*i)) {
			imr.imr_multiaddr.s_addr = inet_addr(s.c_str());
			imr.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef _WIN32
			if (setsockopt(_fdr, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char *)&imr, sizeof(imr)) < 0) {
#else
			if (setsockopt(_fdr, IPPROTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(imr)) < 0) {
#endif
				throw SocketException("Leave group error");
			}

			// std::remove(i);

			break;
		}
	}
}

std::vector<std::string> & MulticastSocket::GetGroupList()
{
	return _groups;
}

void MulticastSocket::Close()
{
	if (_is_closed == false) {
		_is_closed = true;

#ifdef _WIN32
		if (closesocket(_fdr) < 0) {
#else
		if (close(_fdr) != 0) {
#endif
			throw SocketStreamException("Close multicast receiver descriptor error");
		}
	
#ifdef _WIN32
		if (closesocket(_fds) < 0) {
#else
		if (close(_fds) != 0) {
#endif
			throw SocketStreamException("Close multicast sender descriptor error");
		}
	}
}

int MulticastSocket::GetLocalPort()
{
	return ntohs(_sock_r.sin_port);
}

int64_t MulticastSocket::GetSentBytes()
{
	return _sent_bytes + _os->GetSentBytes();
}

int64_t MulticastSocket::GetReadedBytes()
{
	return _receive_bytes + _is->GetReadedBytes();
}

void MulticastSocket::SetMulticastTTL(char ttl_)
{
#ifdef _WIN32

#else
	if (setsockopt(_fds, IPPROTO_IP, IP_MULTICAST_TTL, &ttl_, sizeof(char))) {
		throw SocketException("Seting multicast ttl error");
	}
#endif
}

SocketOption * MulticastSocket::GetSocketOption()
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	return new SocketOption(_fdr, MCAST_SOCKET);
}

}

