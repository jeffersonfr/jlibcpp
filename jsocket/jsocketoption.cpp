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
#include "jsocketoption.h"
#include "jsocketoptionexception.h"

#ifdef _WIN32
#include <windows.h>
#else
#include "jsslsocket.h"
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <netinet/tcp.h>
//#include <linux/tcp.h>
#endif

namespace jsocket {

#ifdef _WIN32
SocketOption::SocketOption(SOCKET fd_, jconnection_type_t type_):
#else
SocketOption::SocketOption(int fd_, jconnection_type_t type_):
#endif
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::SocketOption");
	
	_fd = fd_;
	_type = type_;
}

SocketOption::~SocketOption()
{
}

void SocketOption::SetKeepAlive(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_)?TRUE:FALSE;

	if (setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionException("Set keep alive error");
	}
}

void SocketOption::SetNoDelay(bool b_)
{
#ifdef _WIN32
#else
	int flag = (b_ == false)?0:1;

	// if (setsockopt(_fd, SOL_SOCKET, SO_OOBINLINE, &b_, sizeof(bool)) < 0) {
	if (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int)) < 0) {
		throw SocketOptionException("Set out of band error");
	}
#endif
}

void SocketOption::SetOutOfBandInLine(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_)?TRUE:FALSE;

	if (setsockopt(_fd, SOL_SOCKET, SO_OOBINLINE, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_OOBINLINE, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionException("Set out of band error");
	}
}

void SocketOption::SetSendTimeout(int time_)
{

#ifdef _WIN32
	time_ /= 1000;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time_, sizeof(int)) < 0) {
		throw SocketOptionException("Set send timeout error");
	}
#else
	struct timespec t;

	int time = time_;

	t.tv_sec = (int64_t)(time/1000LL);
	t.tv_nsec = (int64_t)(time%1000LL)*1000;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof(struct timespec)) < 0) {
		throw SocketOptionException("Set send timeout error");
	}
#endif
}

void SocketOption::SetReceiveTimeout(int time_)
{
#ifdef _WIN32
	time_ /= 1000;

	if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_, sizeof(int)) < 0) {
		throw SocketOptionException("Set send timeout error");
	}
#else
	struct timespec t;

	int time = time_;
		
	t.tv_sec = (int64_t)(time/1000LL);
	t.tv_nsec = (int64_t)(time%1000LL)*1000;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timespec)) < 0) {
		throw SocketOptionException("Set receive timeout error");
	}
#endif
}

void SocketOption::SetPassCredentials(bool b_)
{
#ifdef _WIN32
#elif __CYGWIN32__
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_PASSCRED, &b_, sizeof(bool)) < 0) {
		throw SocketOptionException("Set pass credentials error");
	}
#endif
}

void SocketOption::GetPeerCredentials(void *v_)
{
#ifdef _WIN32

#else
	/*
	if (getsockopt(_fd, SOL_SOCKET, SO_PEERCRED, &v_, sizeof(v_)) < 0) {
		throw SocketOptionException("Set peer credentials error");
	}
	*/
#endif
}

void SocketOption::BindToDevice(std::string dev_)
{
#ifdef _WIN32
#elif __CYGWIN32__
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_BINDTODEVICE, dev_.c_str(), dev_.size()+1) < 0) {
		throw SocketOptionException("Bind to device error");
	}
#endif
}

void SocketOption::SetReuseAddress(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_)?TRUE:FALSE;

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionException("Set reuse address error");
	}
}

void SocketOption::SetReusePort(bool b_)
{
#ifdef _WIN32

#else
	#ifndef SO_REUSEPORT
		#define SO_REUSEPORT 15
	#endif
	
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &b_, sizeof(bool)) < 0) {
		throw SocketOptionException("Set reuse port error");
	}
#endif
}

jconnection_type_t SocketOption::GetType()
{
	return _type;
}

bool SocketOption::GetSocketAcceptConnection()
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
		throw SocketOptionException("Get socket accept connection error");
	}

	return b;
}

void SocketOption::SetRoute(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_ == true)?TRUE:FALSE;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_DONTROUTE, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	b_ = (b_ == true)?false:true;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_DONTROUTE, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionException("Set route error");
	}
}

void SocketOption::SetBroadcast(bool b_)
{
#ifdef _WIN32
	BOOL b = (b_)?TRUE:FALSE;

	if (setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, (const char *)&b, sizeof(BOOL)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, &b_, sizeof(bool)) < 0) {
#endif
		throw SocketOptionException("Set broadcast error");
	}
}

void SocketOption::SetSendMaximumBuffer(int length_)
{
#ifdef _WIN32

#else
	length_ /= 2;
#endif

	if (setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, (const char *)&length_, sizeof(int)) < 0) {
		throw SocketOptionException("Set send maximum buffer error");
	}
}

void SocketOption::SetReceiveMaximumBuffer(int length_)
{
#ifdef _WIN32

#else
	length_ /= 2;
#endif
	
	if (setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, (const char *)&length_, sizeof(int)) < 0) {
		throw SocketOptionException("Set receive maximum buffer error");
	}
}

int SocketOption::GetSendMaximumBuffer()
{
	int l = 0,
		length = sizeof(int);

#ifdef _WIN32
	if (getsockopt(_fd, SOL_SOCKET, SO_SNDBUF, (char *)&l, &length) == SOCKET_ERROR) {
#else
	if (getsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &l, (socklen_t *)&length) < 0) {
#endif
		throw SocketOptionException("Get send maximum buffer error");
	}

	return l;
}

int SocketOption::GetReceiveMaximumBuffer()
{
	int l = 0,
		length = sizeof(int);

#ifdef _WIN32
	if (getsockopt(_fd, SOL_SOCKET, SO_RCVBUF, (char *)&l, &length) == SOCKET_ERROR) {
#else
	if (getsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &l, (socklen_t *)&length) < 0) {
#endif
		throw SocketOptionException("Get receive maximum buffer error");
	}

	return l;
}

void SocketOption::SetLinger(bool on, int linger_)
{
	struct linger l;

	l.l_onoff = on;
	l.l_linger = linger_;
	
#ifdef _WIN32
	if (setsockopt(_fd, SOL_SOCKET, SO_LINGER, (const char *)&l, sizeof(l)) < 0) {
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) < 0) {
#endif
		throw SocketOptionException("Set linger error");
	}
}

void SocketOption::SetPriority(int p_)
{
#ifdef _WIN32
#elif __CYGWIN32__
#else
	if (setsockopt(_fd, SOL_SOCKET, SO_PRIORITY, &p_, sizeof(int)) < 0) {
		throw SocketOptionException("Set priority error");
	}
#endif
}

void SocketOption::ClearPendingSocketError()
{
#ifdef _WIN32

#else
	bool b = true;
	
	if (setsockopt(_fd, SOL_SOCKET, SO_ERROR, &b, sizeof(bool)) < 0) {
		throw SocketOptionException("Clean pending socket errors error");
	}
#endif
}

void SocketOption::SetBlocking(bool b)
{
#ifdef _WIN32
#else
	if (b == true) {
		if (fcntl(_fd, F_SETFL, O_SYNC) < 0) {
			throw SocketOptionException("Set socket blocking error");
		}
	} else {
		if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
			throw SocketOptionException("Set socket blocking error");
		}
	}
#endif
}

void SocketOption::SetTypeOfService(int t_)
{
#ifdef _WIN32

#else
	if (setsockopt(_fd, IPPROTO_IP, IP_TOS, (char *)&t_, sizeof(int)) < 0) {
		throw SocketOptionException("Set type of service error");
	}
#endif
}

void SocketOption::SetTimeToLive(int t_)
{
#ifdef _WIN32

#else
	if (setsockopt(_fd, IPPROTO_IP, IP_TTL, (char *)&t_, sizeof(int)) < 0) {
		throw SocketOptionException("Set time to live error");
	}
#endif
}

void SocketOption::SetHeaderInclude(bool b_)
{
#ifdef _WIN32
#elif __CYGWIN32__
#else
	if (setsockopt(_fd, IPPROTO_IP, IP_HDRINCL, &b_, sizeof(bool)) < 0) {
		throw SocketOptionException("Set header include error");
	}
#endif
}

int64_t SocketOption::GetTimeStamp()
{
#ifdef _WIN32
	return 0;
#elif __CYGWIN32__
	return 0;
#else
	struct timespec t;

	if (ioctl(_fd, SIOCGSTAMP, &t) < 0) {
		throw SocketOptionException("Get time stamp error");
	}

	return (t.tv_sec*1000000000LL + t.tv_nsec);
#endif
}

int SocketOption::GetMaximunTransferUnit()
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
		throw SocketOptionException("Get MTU error");
	}
	*/

	return 1500;
#endif
}

void SocketOption::SetIOAsync(bool b_)
{
#ifdef _WIN32

#else
	if (ioctl(_fd, FIOASYNC, b_) < 0) {
		throw SocketOptionException("Set io synchronized error");
	}
#endif
}

void SocketOption::SetMulticastLoop(bool b_)
{
#ifdef _WIN32

#else
	if (_type != MCAST_SOCKET) {
		return;
	}
	
	if (setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &b_, sizeof(bool)) < 0) {
		throw SocketOptionException("Set multicast loop error");
	}
#endif
}

void SocketOption::SetRSVP(int t_)
{
#ifdef _WIN32

#else
	if (_type != MCAST_SOCKET) {
		return;
	}

	/*
	if (setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_VIF, &t_, sizeof(int)) < 0) {
		throw SocketOptionException("Set rsvp error");
	}
	*/
#endif
}

void SocketOption::SetShutdown(socket_shutdown_t opt_)
{
#ifdef _WIN32
#else
	if (_type != MCAST_SOCKET) {
		if (shutdown(_fd, opt_) < 0) {
			throw SocketOptionException("Shutdown socket error");
		}
	} else {
		if ((opt_ & SHUTDOWN_READ) != 0 || (opt_ & SHUTDOWN_READ_WRITE) != 0) {
			if (shutdown(_fd, opt_) < 0) {
				throw SocketOptionException("Shutdown multicast socket error");
			}
		}
	}
#endif
}

}

