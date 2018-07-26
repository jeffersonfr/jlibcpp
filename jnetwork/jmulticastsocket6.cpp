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
#include "jnetwork/jmulticastsocket6.h"
#include "jnetwork/jinetaddress6.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"

#include <poll.h>
#include <unistd.h>
#include <string.h>

namespace jnetwork {

MulticastSocket6::MulticastSocket6(std::string host_, int port_, int rbuf_, int wbuf_):
	jnetwork::Connection(JCT_MCAST)
{
	jcommon::Object::SetClassName("jnetwork::MulticastSocket6");
	
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
	if ((_fds = ::socket(PF_INET6, SOCK_DGRAM, IPPROTO_IPV6)) < 0) { // IPPROTO_MTP
		throw jexception::ConnectionException("Socket create::sender exception");
	}

	if ((_fdr = ::socket(PF_INET6, SOCK_DGRAM, IPPROTO_IPV6)) < 0) { // IPPROTO_MTP
		throw jexception::ConnectionException("Socket create::receiver exception");
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
		throw jexception::ConnectionException("Binding error");
	}
}

void MulticastSocket6::InitStream(int rbuf_, int wbuf_)
{
	_is = new SocketInputStream((Connection *)this, (struct sockaddr *)&_sock_r, rbuf_);
	_os = new SocketOutputStream((Connection *)this, (struct sockaddr *)&_sock_s, wbuf_);
}

/** End */

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
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	struct pollfd ufds[1];

	ufds[0].fd = _fdr;
	ufds[0].events = POLLIN | POLLRDBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw jexception::ConnectionException("Invalid receive parameters exception");
	} else if (rv == 0) {
		throw jexception::ConnectionTimeoutException("Socket input timeout error");
	} else {
		if ((ufds[0].revents & POLLIN) || (ufds[0].revents & POLLRDBAND)) {
			return MulticastSocket6::Receive(data_, size_);
		}
	}

	return -1;
}

int MulticastSocket6::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}

	int n,
			flags = 0,
			length = sizeof(_sock_r);

	if (block_ == false) {
		flags = MSG_DONTWAIT;
	}

	n = ::recvfrom(_fdr, data_, size_, flags, (struct sockaddr *)&_sock_r, (socklen_t *)&length);
	
	if (n < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (block_ == true) {
				throw jexception::ConnectionTimeoutException("Socket input timeout error");
			}
		}
			
		throw jexception::IOException("Socket input error");
	} else if (n == 0) {
		if (block_ == true) {
			Close();

			throw jexception::IOException("Peer has shutdown");
		}
	}

	/*
	if (n < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (block_ == true) {
				throw jexception::ConnectionTimeoutException("Socket input timeout error");
			} else {
				// INFO:: non-blocking socket, no data read
				n = 0;
			}
		} else {
			throw jexception::IOException("Socket input error");
		}
	} else if (n == 0) {
		Close();

		throw jexception::IOException("Peer shutdown exception");
	}
	*/

	_receive_bytes += n;

	return n;
}

int MulticastSocket6::Send(const char *data, int size, int time_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}

	struct pollfd ufds[1];

	ufds[0].fd = _fds;
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw jexception::ConnectionException("Invalid send parameters exception");
	} else if (rv == 0) {
		throw jexception::ConnectionTimeoutException("Socket output timeout error");
	} else {
		if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLWRBAND)) {
			return MulticastSocket6::Send(data, size);
		}
	}

	return -1;
}

int MulticastSocket6::Send(const char *data, int size, bool block_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}

	int flags;

	if (block_ == true) {
		flags = MSG_NOSIGNAL;
	} else {
		flags = MSG_NOSIGNAL | MSG_DONTWAIT;
	}

	int n = ::sendto(_fds, data, size, flags, (struct sockaddr *)&_sock_s, sizeof(_sock_s));

	if (n < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (block_ == true) {
				throw jexception::ConnectionTimeoutException("Socket output timeout error");
			}
				
			throw jexception::ConnectionException("Socket output exception");
			
			/*
			if (block_ == true) {
				throw jexception::ConnectionTimeoutException("Socket output timeout error");
			} else {
				// INFO:: non-blocking socket, no data read
				n = 0;
			}
			*/
		} else if (errno == EPIPE || errno == ECONNRESET) {
			Close();

			throw jexception::ConnectionException("Broken pipe exception");
		} else {
			throw jexception::ConnectionTimeoutException("Socket output timeout error");
		}
	}

	_sent_bytes += n;
	
	return n;
}

void MulticastSocket6::Join(std::string group_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	struct ip_mreq imr;

	imr.imr_multiaddr.s_addr = inet_addr(group_.c_str());
	imr.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_JOIN_GROUP, &imr, sizeof(imr)) < 0) {
		throw jexception::ConnectionException("MulticastSocket join exception");
	}

	_groups.push_back(group_);
}

void MulticastSocket6::Join(InetAddress *group_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	struct ip_mreq imr;

	imr.imr_multiaddr.s_addr = inet_addr(group_->GetHostAddress().c_str());
	imr.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_JOIN_GROUP, &imr, sizeof(imr)) < 0) {
		throw jexception::ConnectionException("MulticastSocket join exception");
	}

	_groups.push_back(group_->GetHostAddress());
}

void MulticastSocket6::Leave(std::string group_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	struct ip_mreq imr;

	for (std::vector<std::string>::iterator i=_groups.begin(); i!=_groups.end(); i++) {
		if (group_ == (*i)) {
			imr.imr_multiaddr.s_addr = inet_addr(group_.c_str());
			imr.imr_interface.s_addr = htonl(INADDR_ANY);

			if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &imr, sizeof(imr)) < 0) {
				throw jexception::ConnectionException("MulticastSocket leave exception");
			}

			// _groups.remove(*i);

			break;
		}
	}
}

void MulticastSocket6::Leave(InetAddress *group_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	struct ip_mreq imr;
	std::string s = group_->GetHostAddress();

	for (std::vector<std::string>::iterator i=_groups.begin(); i!=_groups.end(); i++) {
		if (s == (*i)) {
			imr.imr_multiaddr.s_addr = inet_addr(s.c_str());
			imr.imr_interface.s_addr = htonl(INADDR_ANY);

			if (setsockopt(_fdr, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &imr, sizeof(imr)) < 0) {
				throw jexception::ConnectionException("MulticastSocket leave exception");
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

	if (close(_fdr) != 0) {
		flag = true;
	}
	
	if (close(_fds) != 0) {
		flag = true;
	}
	
	if (flag == true) {
		throw jexception::IOException("Unknown close exception");
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
	if (setsockopt(_fds, IPPROTO_IPV6, IP_MULTICAST_TTL, &ttl_, sizeof(char))) {
		throw jexception::ConnectionException("Seting multicast ttl error");
	}
}

SocketOptions * MulticastSocket6::GetReadSocketOptions()
{
	return new SocketOptions(_fdr, JCT_MCAST);
}

SocketOptions * MulticastSocket6::GetWriteSocketOptions()
{
	return new SocketOptions(_fds, JCT_MCAST);
}

}

