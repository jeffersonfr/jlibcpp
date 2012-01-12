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
#include "jmulticastsocket6.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jioexception.h"
#include "jinetaddress6.h"

namespace jsocket {

int MulticastSocket6::_used_port = 1024;

MulticastSocket6::MulticastSocket6(std::string host_, int port_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_MCAST)
{
	jcommon::Object::SetClassName("jsocket::MulticastSocket6");
	
	_is = NULL;
	_os = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;

	CreateSocket();
	ConnectSocket(InetAddress6::GetByName(host_), port_);
	BindSocket(InetAddress6::GetByName(host_), port_);
	Join(InetAddress6::GetByName(host_));
	InitStream(rbuf_, wbuf_);
}

MulticastSocket6::~MulticastSocket6()
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

void MulticastSocket6::CreateSocket()
{
#ifdef _WIN32
	if ((_fds = ::socket(PF_INET6, SOCK_DGRAM, IPPROTO_IPV6)) == INVALID_SOCKET) { // IPPROTO_MTP
#else
	if ((_fds = ::socket(PF_INET6, SOCK_DGRAM, IPPROTO_IPV6)) < 0) { // IPPROTO_MTP
#endif
		throw SocketException("Socket create::sender exception");
	}

#ifdef _WIN32
	if ((_fdr = ::socket(PF_INET6, SOCK_DGRAM, IPPROTO_IPV6)) == INVALID_SOCKET) { // IPPROTO_MTP
#else
	if ((_fdr = ::socket(PF_INET6, SOCK_DGRAM, IPPROTO_IPV6)) < 0) { // IPPROTO_MTP
#endif
		throw SocketException("Socket create::receiver exception");
	}

	_is_closed = false;
}

void MulticastSocket6::ConnectSocket(InetAddress *addr_, int port_)
{
	// Receive
	memset(&_sock_r, 0, sizeof(_sock_r));

	_sock_r.sin6_family = AF_INET6;
	_sock_r.sin6_port = htons(port_);
	_sock_r.sin6_flowinfo = 0;
	_sock_r.sin6_scope_id = 0;

	if (addr_ == NULL) {
		_sock_r.sin6_addr = in6addr_any;
	} else {
		inet_pton(AF_INET6, addr_->GetHostAddress().c_str(), &(_sock_r.sin6_addr));
	}

	// Send
	memset(&_sock_s, 0, sizeof(_sock_s));

	_sock_s.sin6_family = AF_INET6;
	_sock_s.sin6_port = htons(port_);
	_sock_s.sin6_flowinfo = 0;
	_sock_s.sin6_scope_id = 0;
	_sock_s.sin6_addr = in6addr_any;
}

void MulticastSocket6::BindSocket(InetAddress *addr_, int local_port_)
{
	if (bind(_fdr, (struct sockaddr *)&_sock_r, sizeof(_sock_r)) < 0) {
		throw SocketException("Socket bind exception");
	}
}

void MulticastSocket6::InitStream(int rbuf_, int wbuf_)
{
	_is = new SocketInputStream((Connection *)this, &_is_closed, (struct sockaddr *)&_sock_r, rbuf_);
	_os = new SocketOutputStream((Connection *)this, &_is_closed, (struct sockaddr *)&_sock_s, wbuf_);
}

/** End */

jsocket_t MulticastSocket6::GetHandler()
{
	return _fdr;
}

jio::InputStream * MulticastSocket6::GetInputStream()
{
	return (jio::InputStream *)_is;
}

jio::OutputStream * MulticastSocket6::GetOutputStream()
{
	return (jio::OutputStream *)_os;
}

int MulticastSocket6::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}
	
#ifdef _WIN32
	return MulticastSocket6::Receive(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fdr;
	ufds[0].events = POLLIN | POLLRDBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Invalid receive parameters exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket read timeout exception");
	} else {
		if ((ufds[0].revents & POLLIN) || (ufds[0].revents & POLLRDBAND)) {
			return MulticastSocket6::Receive(data_, size_);
		}
	}
#endif

	return -1;
}

int MulticastSocket6::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
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
		throw jio::IOException("Peer shutdown exception");
	}

	_receive_bytes += n;

	return n;
}

int MulticastSocket6::Send(const char *data, int size, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

#ifdef _WIN32
	return MulticastSocket6::Send(data, size);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _fds;
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Invalid send parameters exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket send timeout exception");
	} else {
		if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLWRBAND)) {
			return MulticastSocket6::Send(data, size);
		}
	}
#endif

	return -1;
}

int MulticastSocket6::Send(const char *data, int size, bool block_)
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

	int n = ::sendto(_fds, data, size, flags, (struct sockaddr *)&_sock_s, sizeof(_sock_s));

#ifdef _WIN32
	if (n == SOCKET_ERROR) {
		throw SocketException("Send udp data error");
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
		} else {
			throw SocketTimeoutException("Socket send exception");
		}
	}
#endif

	_sent_bytes += n;
	
	return n;
}

void MulticastSocket6::Join(std::string group_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}
	
	struct ip_mreq imr;

	imr.imr_multiaddr.s_addr = inet_addr(group_.c_str());
	imr.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef _WIN32
	if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_JOIN_GROUP, (const char *)&imr, sizeof(imr)) < 0) {
#else
	if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_JOIN_GROUP, &imr, sizeof(imr)) < 0) {
#endif
		throw SocketException("MulticastSocket join exception");
	}

	_groups.push_back(group_);
}

void MulticastSocket6::Join(InetAddress *group_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}
	
	struct ip_mreq imr;

	imr.imr_multiaddr.s_addr = inet_addr(group_->GetHostAddress().c_str());
	imr.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef _WIN32
	if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_JOIN_GROUP, (const char *)&imr, sizeof(imr)) < 0) {
#else
	if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_JOIN_GROUP, &imr, sizeof(imr)) < 0) {
#endif
		throw SocketException("MulticastSocket join exception");
	}

	_groups.push_back(group_->GetHostAddress());
}

void MulticastSocket6::Leave(std::string group_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}
	
	struct ip_mreq imr;

	for (std::vector<std::string>::iterator i=_groups.begin(); i!=_groups.end(); i++) {
		if (group_ == (*i)) {
			imr.imr_multiaddr.s_addr = inet_addr(group_.c_str());
			imr.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef _WIN32
			if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_LEAVE_GROUP, (const char *)&imr, sizeof(imr)) < 0) {
#else
			if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &imr, sizeof(imr)) < 0) {
#endif
				throw SocketException("MulticastSocket leave exception");
			}

			// _groups.remove(*i);

			break;
		}
	}
}

void MulticastSocket6::Leave(InetAddress *group_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}
	
	struct ip_mreq imr;
	std::string s = group_->GetHostAddress();

	for (std::vector<std::string>::iterator i=_groups.begin(); i!=_groups.end(); i++) {
		if (s == (*i)) {
			imr.imr_multiaddr.s_addr = inet_addr(s.c_str());
			imr.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef _WIN32
			if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_LEAVE_GROUP, (const char *)&imr, sizeof(imr)) < 0) {
#else
			if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &imr, sizeof(imr)) < 0) {
#endif
				throw SocketException("MulticastSocket leave exception");
			}

			// std::remove(i);

			break;
		}
	}
}

std::vector<std::string> & MulticastSocket6::GetGroupList()
{
	return _groups;
}

void MulticastSocket6::Close()
{
	if (_is_closed == true) {
		return;
	}

	bool flag = false;

#ifdef _WIN32
	if (closesocket(_fdr) < 0) {
#else
	if (close(_fdr) != 0) {
#endif
		flag = true;
	}
	
#ifdef _WIN32
	if (closesocket(_fds) < 0) {
#else
	if (close(_fds) != 0) {
#endif
		flag = true;
	}
	
	if (flag == true) {
		throw jio::IOException("Unknown close exception");
	}

	_is_closed = true;
}

int MulticastSocket6::GetLocalPort()
{
	return ntohs(_sock_r.sin6_port);
}

int64_t MulticastSocket6::GetSentBytes()
{
	return _sent_bytes + _os->GetSentBytes();
}

int64_t MulticastSocket6::GetReadedBytes()
{
	return _receive_bytes + _is->GetReadedBytes();
}

void MulticastSocket6::SetMulticastTTL(char ttl_)
{
#ifdef _WIN32
#else
	if (setsockopt(_fds, IPPROTO_IPV6, IP_MULTICAST_TTL, &ttl_, sizeof(char))) {
		throw SocketException("Seting multicast ttl error");
	}
#endif
}

SocketOptions * MulticastSocket6::GetSocketOptions()
{
	return new SocketOptions(_fdr, JCT_MCAST);
}

SocketOptions * MulticastSocket6::GetSocketOptionsExtension()
{
	return new SocketOptions(_fds, JCT_MCAST);
}

}

