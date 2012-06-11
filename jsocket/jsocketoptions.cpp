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
#include "jsocketoptions.h"
#include "jsocketoptionsexception.h"

namespace jsocket {

SocketOptions::SocketOptions(jsocket_t fd_, jconnection_type_t type_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::SocketOptions");
	
	_fd = fd_;
	_type = type_;
}

SocketOptions::~SocketOptions()
{
}

void SocketOptions::SetKeepAlive(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_)?TRUE:FALSE;

	if (setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionsException("Set keep alive error");
	}
}

void SocketOptions::SetNoDelay(bool b_)
{
#ifdef _WIN32
#else
	int flag = (b_ == false)?0:1;

	// if (setsockopt(_fd, SOL_SOCKET, SO_OOBINLINE, &b_, sizeof(bool)) < 0) {
	if (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int)) < 0) {
		throw SocketOptionsException("Set out of band error");
	}
#endif
}

void SocketOptions::SetOutOfBandInLine(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_)?TRUE:FALSE;

	if (setsockopt(_fd, SOL_SOCKET, SO_OOBINLINE, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_OOBINLINE, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionsException("Set out of band error");
	}
}

void SocketOptions::SetSendTimeout(int time_)
{
#ifdef _WIN32
	if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time_, sizeof(int)) < 0) {
		throw SocketOptionsException("Set send timeout error");
	}
#else
	struct timespec t;

	int time = time_;

	t.tv_sec = (int64_t)(time/1000LL);
	t.tv_nsec = (int64_t)(time%1000LL)*1000LL;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof(struct timespec)) < 0) {
		throw SocketOptionsException("Set send timeout error");
	}
#endif
}

void SocketOptions::SetReceiveTimeout(int time_)
{
#ifdef _WIN32
	if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_, sizeof(time_)) < 0) {
		throw SocketOptionsException("Set send timeout error");
	}
#else
	struct timespec t;

	int time = time_;
		
	t.tv_sec = (int64_t)(time/1000LL);
	t.tv_nsec = (int64_t)(time%1000LL)*1000LL;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timespec)) < 0) {
		throw SocketOptionsException("Set receive timeout error");
	}
#endif
}

void SocketOptions::SetPassCredentials(bool b_)
{
#ifdef _WIN32
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_PASSCRED, &b_, sizeof(bool)) < 0) {
		throw SocketOptionsException("Set pass credentials error");
	}
#endif
}

void SocketOptions::GetPeerCredentials(void *v_)
{
#ifdef _WIN32
#else
	/*
	if (getsockopt(_fd, SOL_SOCKET, SO_PEERCRED, &v_, sizeof(v_)) < 0) {
		throw SocketOptionsException("Set peer credentials error");
	}
	*/
#endif
}

void SocketOptions::BindToDevice(std::string dev_)
{
#ifdef _WIN32
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_BINDTODEVICE, dev_.c_str(), dev_.size()+1) < 0) {
		throw SocketOptionsException("Bind to device error");
	}
#endif
}

void SocketOptions::SetReuseAddress(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_)?TRUE:FALSE;

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionsException("Set reuse address error");
	}
}

void SocketOptions::SetReusePort(bool b_)
{
#ifdef _WIN32
#else
	#ifndef SO_REUSEPORT
		#define SO_REUSEPORT 15
	#endif
	
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &b_, sizeof(bool)) < 0) {
		throw SocketOptionsException("Set reuse port error");
	}
#endif
}

jconnection_type_t SocketOptions::GetType()
{
	return _type;
}

bool SocketOptions::GetSocketAcceptConnection()
{
#ifdef _WIN32
	int length = sizeof(BOOL);
	BOOL b;
	
	if (getsockopt(_fd, SOL_SOCKET, SO_ACCEPTCONN, (char *)&b, (int *)&length) < 0) {
#else
	socklen_t length = sizeof(bool);
	bool b;
	
	if (getsockopt(_fd, SOL_SOCKET, SO_ACCEPTCONN, (char *)&b, &length) < 0) {
#endif
		throw SocketOptionsException("Get socket accept connection error");
	}

#ifdef _WIN32
	return b != 0;
#else
	return b;
#endif
}

void SocketOptions::SetRoute(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_ == true)?TRUE:FALSE;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_DONTROUTE, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	b_ = (b_ == true)?false:true;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_DONTROUTE, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionsException("Set route error");
	}
}

void SocketOptions::SetBroadcast(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_)?TRUE:FALSE;

	if (setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionsException("Set broadcast error");
	}
}

void SocketOptions::SetSendMaximumBuffer(int length_)
{
#ifdef _WIN32
#else
	length_ /= 2;
#endif

	if (setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, (const char *)&length_, sizeof(int)) < 0) {
		throw SocketOptionsException("Set send maximum buffer error");
	}
}

void SocketOptions::SetReceiveMaximumBuffer(int length_)
{
#ifdef _WIN32
#else
	length_ /= 2;
#endif
	
	if (setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, (const char *)&length_, sizeof(int)) < 0) {
		throw SocketOptionsException("Set receive maximum buffer error");
	}
}

int SocketOptions::GetSendMaximumBuffer()
{
	int l = 0,
		length = sizeof(int);

#ifdef _WIN32
	if (getsockopt(_fd, SOL_SOCKET, SO_SNDBUF, (char *)&l, &length) == SOCKET_ERROR) {
#else
	if (getsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &l, (socklen_t *)&length) < 0) {
#endif
		throw SocketOptionsException("Get send maximum buffer error");
	}

	return l;
}

int SocketOptions::GetReceiveMaximumBuffer()
{
	int l = 0,
		length = sizeof(int);

#ifdef _WIN32
	if (getsockopt(_fd, SOL_SOCKET, SO_RCVBUF, (char *)&l, &length) == SOCKET_ERROR) {
#else
	if (getsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &l, (socklen_t *)&length) < 0) {
#endif
		throw SocketOptionsException("Get receive maximum buffer error");
	}

	return l;
}

void SocketOptions::SetLinger(bool on, int linger_)
{
	struct linger l;

	l.l_onoff = on;
	l.l_linger = linger_;
	
#ifdef _WIN32
	if (setsockopt(_fd, SOL_SOCKET, SO_LINGER, (const char *)&l, sizeof(l)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) < 0) {
#endif
		throw SocketOptionsException("Set linger error");
	}
}

void SocketOptions::SetPriority(int p_)
{
#ifdef _WIN32
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_PRIORITY, &p_, sizeof(int)) < 0) {
		throw SocketOptionsException("Set priority error");
	}
#endif
}

void SocketOptions::ClearPendingSocketError()
{
#ifdef _WIN32
#else
	bool b = true;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_ERROR, &b, sizeof(bool)) < 0) {
		throw SocketOptionsException("Clean pending socket errors error");
	}
#endif
}

void SocketOptions::SetBlocking(bool b)
{
#ifdef _WIN32
#else
	if (b == true) {
		if (fcntl(_fd, F_SETFL, O_SYNC) < 0) {
			throw SocketOptionsException("Set socket blocking error");
		}
	} else {
		if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
			throw SocketOptionsException("Set socket blocking error");
		}
	}
#endif
}

void SocketOptions::SetTypeOfService(int t_)
{
#ifdef _WIN32
#else
	if (setsockopt(_fd, IPPROTO_IP, IP_TOS, (char *)&t_, sizeof(int)) < 0) {
		throw SocketOptionsException("Set type of service error");
	}
#endif
}

void SocketOptions::SetTimeToLive(int t_)
{
#ifdef _WIN32
#else
	if (setsockopt(_fd, IPPROTO_IP, IP_TTL, (char *)&t_, sizeof(int)) < 0) {
		throw SocketOptionsException("Set time to live error");
	}
#endif
}

void SocketOptions::SetHeaderInclude(bool b_)
{
#ifdef _WIN32
#else
	if (setsockopt(_fd, IPPROTO_IP, IP_HDRINCL, &b_, sizeof(bool)) < 0) {
		throw SocketOptionsException("Set header include error");
	}
#endif
}

int64_t SocketOptions::GetTimeStamp()
{
#ifdef _WIN32
	return 0LL;
#else
	struct timespec t;

	if (ioctl(_fd, SIOCGSTAMP, &t) < 0) {
		throw SocketOptionsException("Get time stamp error");
	}

	return (t.tv_sec*1000000000LL + t.tv_nsec);
#endif
}

int SocketOptions::GetMaximunTransferUnit()
{
#ifdef _WIN32
	return 1500;
#else
	/*
	socklen_t length = 4;
	int mtu = 1500;
	
#ifndef IP_MTU
#define IP_MTU	14
#endif
	
	if (getsockopt(_fd, IPPROTO_IP, IP_MTU, (void *)&mtu, (socklen_t *)length) < 0) {
		throw SocketOptionsException("Get MTU error");
	}
	*/

	return 1500;
#endif
}

void SocketOptions::SetIOAsync(bool b_)
{
#ifdef _WIN32
#else
	if (ioctl(_fd, FIOASYNC, b_) < 0) {
		throw SocketOptionsException("Set io synchronized error");
	}
#endif
}

void SocketOptions::SetMulticastLoop(bool b_)
{
#ifdef _WIN32
#else
	if (_type != JCT_MCAST) {
		return;
	}
	
	if (setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &b_, sizeof(bool)) < 0) {
		throw SocketOptionsException("Set multicast loop error");
	}
#endif
}

void SocketOptions::SetRSVP(int t_)
{
#ifdef _WIN32
#else
	if (_type != JCT_MCAST) {
		return;
	}

	/*
	if (setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_VIF, &t_, sizeof(int)) < 0) {
		throw SocketOptionsException("Set rsvp error");
	}
	*/
#endif
}

void SocketOptions::SetShutdown(socket_shutdown_t opt_)
{
#ifdef _WIN32
#else
	if (_type != JCT_MCAST) {
		if (shutdown(_fd, opt_) < 0) {
			throw SocketOptionsException("Shutdown socket error");
		}
	} else {
		if ((opt_ & SHUTDOWN_READ) != 0 || (opt_ & SHUTDOWN_READ_WRITE) != 0) {
			if (shutdown(_fd, opt_) < 0) {
				throw SocketOptionsException("Shutdown multicast socket error");
			}
		}
	}
#endif
}

void SocketOptions::SetIPv6UnicastHops(int opt_)
{
#ifdef _WIN32
#else
	if (setsockopt(_fd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &opt_, sizeof(int)) < 0) {
		throw SocketOptionsException("Set ipv6 socket hops error");
	}
#endif
}

int SocketOptions::GetIPv6UnicastHops()
{
#ifdef _WIN32
	return -1;
#else
	socklen_t length = sizeof(int);
	int opt;

	if (getsockopt(_fd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &opt, &length) < 0) {
		throw SocketOptionsException("Set ipv6 socket hops error");
	}
	
	return opt;
#endif
}

void SocketOptions::SetIPv6Only(bool opt_)
{
#ifdef _WIN32
#else
	if (setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt_, sizeof(bool)) < 0) {
		throw SocketOptionsException("Set ipv6 only error");
	}
#endif
}

}

