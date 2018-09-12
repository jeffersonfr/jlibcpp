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
#include "jnetwork/jsocket.h"
#include "jnetwork/jinetaddress4.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"

#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

namespace jnetwork {

Socket::Socket(InetAddress *addr_, int port_, int timeout_, int rbuf_, int wbuf_):
	jnetwork::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jnetwork::Socket");
	
	_is = nullptr;
	_os = nullptr;
	_address = nullptr;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	CreateSocket();
	ConnectSocket(addr_, port_);
	InitStreams(rbuf_, wbuf_);
}

Socket::Socket(InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, int timeout_, int rbuf_, int wbuf_):
	jnetwork::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jnetwork::Socket");

	_is = nullptr;
	_os = nullptr;
	_address = nullptr;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	CreateSocket();
	BindSocket(local_addr_, local_port_);
	ConnectSocket(addr_, port_);
	InitStreams(rbuf_, wbuf_);
}

Socket::Socket(std::string host_, int port_, int timeout_, int rbuf_, int wbuf_):
	jnetwork::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jnetwork::Socket");

	_is = nullptr;
	_os = nullptr;
	_address = nullptr;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	InetAddress *address = InetAddress4::GetByName(host_);

	CreateSocket();
	ConnectSocket(address, port_);
	InitStreams(rbuf_, wbuf_);
}

Socket::Socket(std::string host_, int port_, InetAddress *local_addr_, int local_port_, int timeout_, int rbuf_, int wbuf_):
	jnetwork::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jnetwork::Socket");

	_is = nullptr;
	_os = nullptr;
	_address = nullptr;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	InetAddress *address = InetAddress4::GetByName(host_);

	CreateSocket();
	BindSocket(local_addr_, local_port_);
	ConnectSocket(address, port_);
	InitStreams(rbuf_, wbuf_);
}

Socket::~Socket()
{
	try {
		Close();
	} catch (...) {
	}

	if (_is != nullptr) {
		delete _is;
		_is = nullptr;
	}

	if (_os != nullptr) {
		delete _os;
		_os = nullptr;
	}

	if (_address != nullptr) {
		delete _address;
		_address = nullptr;
	}
}

/** Private */

Socket::Socket(int fd_, struct sockaddr_in server_, int timeout_, int rbuf_, int wbuf_):
	jnetwork::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jnetwork::Socket");

	// socklen_t len;

	_fd = fd_;
	_is_closed = false;

	_lsock.sin_family = AF_INET;
	_server_sock = server_;

	_address = InetAddress4::GetByName((std::string)inet_ntoa(server_.sin_addr));

	InitStreams(rbuf_, wbuf_);
}

void Socket::CreateSocket()
{
	if ((_fd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		throw jexception::ConnectionException("Socket handling error");
	}

	_is_closed = false;
}

void Socket::BindSocket(InetAddress *local_addr_, int local_port_)
{
	memset(&_lsock, 0, sizeof(_lsock));

	_lsock.sin_family = AF_INET;

	if (local_addr_ == nullptr) {
		_lsock.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		_local = local_addr_;

		_lsock.sin_addr.s_addr = inet_addr(local_addr_->GetHostAddress().c_str());
		// memcpy(&(_lsock.sin_addr.s_addr), &(_local->_ip), sizeof(_local->_ip));
	}

	_lsock.sin_port = htons(local_port_);

	if (bind(_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) < 0) {
		throw jexception::ConnectionException("Binding error");
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

	if (_timeout > 0) {
		long arg;

		if( (arg = fcntl(_fd, F_GETFL, nullptr)) < 0) { 
			throw jexception::ConnectionException("Cannont set non blocking socket");
		}

		arg |= O_NONBLOCK; 

		if( fcntl(_fd, F_SETFL, arg) < 0) { 
			throw jexception::ConnectionException("Cannont set non blocking socket");
		} 

		r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));

		if (r < 0) {
			if (errno == EINPROGRESS) { 
				// EINPROGRESS in connect() - selecting
				do { 
					struct timeval tv; 
					fd_set wset;

					tv.tv_sec = _timeout/1000;
					tv.tv_usec = (_timeout%1000)*1000;

					FD_ZERO(&wset); 
					FD_SET(_fd, &wset); 

					r = select(_fd+1, nullptr, &wset, nullptr, &tv); 

					if (r < 0 && errno != EINTR) { 
						throw jexception::ConnectionException("Connection error");
					} else if (r > 0) { 
						socklen_t len = sizeof(int); 
						int val; 
						
						if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void*)(&val), &len) < 0) { 
							throw jexception::ConnectionException("Unknown error in getsockopt()");
						} 

						if (val) { 
							throw jexception::ConnectionException("Error in delayed connection");
						}

						break; 
					} else { 
						throw jexception::ConnectionException("Socket connection timeout exception");
					} 
				} while (true); 
			} else { 
				throw jexception::ConnectionException("Unknown error");
			} 
		}
	} else {
		r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
	}

	if (r < 0) {
		throw jexception::ConnectionException("Connection error");
	}
}

void Socket::InitStreams(int64_t rbuf_, int64_t wbuf_)
{
	_is = new SocketInputStream((Connection *)this, rbuf_);
	_os = new SocketOutputStream((Connection *)this, wbuf_);
}

/** End */

int Socket::Send(const char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}

	struct pollfd ufds[1];

	ufds[0].fd = _fd;
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw jexception::ConnectionException("Invalid send parameters exception");
	} else if (rv == 0) {
		throw jexception::ConnectionTimeoutException("Socket output timeout error");
	} else {
		if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLWRBAND)) {
			return Socket::Send(data_, size_);
		}
	}

	return -1;
}

int Socket::Send(const char *data_, int size_, bool block_)
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

	int n = ::send(_fd, data_, size_, flags);

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

int Socket::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}

	struct pollfd ufds[1];

	ufds[0].fd = _fd;
	ufds[0].events = POLLIN | POLLRDBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw jexception::ConnectionException("Invalid receive parameters exception");
	} else if (rv == 0) {
		throw jexception::ConnectionTimeoutException("Socket input timeout error");
	} else {
		if ((ufds[0].revents & POLLIN) || (ufds[0].revents & POLLRDBAND)) {
			return Socket::Receive(data_, size_);
		}
	}

	return -1;
}

int Socket::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}

	int flags = 0;

	if (block_ == false) {
		flags = MSG_DONTWAIT;
	}

	int n = ::recv(_fd, data_, size_, flags);

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
			throw jconnection::IOException("Socket input error");
		}
	} else if (n == 0) {
		Close();

		throw jconnection::IOException("Peer has shutdown");
	}
	*/

	_receive_bytes += n;

	return n;
}

void Socket::Close()
{
	if (_is_closed == true) {
		return;
	}

	if (close(_fd) != 0) {
		throw jexception::ConnectionException("Unknown close exception");
	}

	_is_closed = true;
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

int64_t Socket::GetReadedBytes()
{
	return _receive_bytes + _is->GetReadedBytes();
}

SocketOptions * Socket::GetSocketOptions()
{
	return new SocketOptions(_fd, JCT_TCP);
}

std::string Socket::What()
{
	char *port = (char *)malloc(10);

	sprintf(port, "%u", GetPort());

	return GetInetAddress()->GetHostName() + ":" + port;
}

}
