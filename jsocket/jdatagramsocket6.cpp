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
#include "jioexception.h"
#include "junknownhostexception.h"
#include "jinetaddress6.h"

namespace jsocket {

int DatagramSocket6::_used_port = 1024;

DatagramSocket6::DatagramSocket6(std::string host_, int port_, bool stream_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_UDP)
{
#ifdef _WIN32
#else
	jcommon::Object::SetClassName("jsocket::DatagramSocket6");
	
	_stream = stream_;
	_address = NULL;
	_is = NULL;
	_os = NULL;
	_is_closed = true;
	_timeout = timeout_;
	
	_address = InetAddress6::GetByName(host_);

	CreateSocket();
	ConnectSocket(InetAddress6::GetByName(host_), port_);
	InitStream(rbuf_, wbuf_);

	_sent_bytes = 0;
	_receive_bytes = 0;
#endif
}

DatagramSocket6::DatagramSocket6(int port_, bool stream_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_UDP)
{
	jcommon::Object::SetClassName("jsocket::DatagramSocket6");

#ifdef _WIN32
#else
	_address = NULL;
	_is = NULL;
	_os = NULL;
	_is_closed = true;
	_stream = stream_;
	_timeout = timeout_;

	_address = InetAddress6::GetByName("127.0.0.1");

	CreateSocket();

#ifdef _WIN32
	if (port_ == 0) {
		for (int i=1024; i<65535; i++) {
			try {
				BindSocket(NULL, i);
			
				break;
			} catch(SocketException &) {
			}
		}
	} else
#else
		BindSocket(NULL, port_);
#endif

	InitStream(rbuf_, wbuf_);

	_sent_bytes = 0;
	_receive_bytes = 0;
#endif
}

DatagramSocket6::DatagramSocket6(InetAddress *addr_, int port_, bool stream_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_UDP)
{
	jcommon::Object::SetClassName("jsocket::DatagramSocket6");

#ifdef _WIN32
#else
	_address = addr_;
	_is = NULL;
	_os = NULL;
	_is_closed = true;
	_stream = stream_;
	_timeout = timeout_;

	CreateSocket();

#ifdef _WIN32
	if (port_ == 0) {
		for (int i=1024; i<65535; i++) {
			try {
				BindSocket(_address, i);
			
				break;
			} catch(SocketException &) {
			}
		}
	} else
#else
		BindSocket(_address, port_);
#endif

	InitStream(rbuf_, wbuf_);

	_sent_bytes = 0;
	_receive_bytes = 0;
#endif
}

DatagramSocket6::~DatagramSocket6()
{
#ifdef _WIN32
#else
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
#endif
}

/** Private */

void DatagramSocket6::CreateSocket()
{
#ifdef _WIN32
#else
	if ((_fd = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		throw SocketException("Socket handling error");
	}

	_is_closed = false;
#endif
}

void DatagramSocket6::BindSocket(InetAddress *local_addr_, int local_port_)
{
#ifdef _WIN32
#else
	int opt = 1;

	memset(&_lsock, 0, sizeof(_lsock));
   
	_lsock.sin6_family = AF_INET6;
	_lsock.sin6_flowinfo = 0;
	
	if (local_addr_ == NULL) {
		_lsock.sin6_addr = in6addr_any;
	} else {
		_local = dynamic_cast<InetAddress6 *>(local_addr_);

		memcpy(&(_lsock.sin6_addr), &(_local->_ip), sizeof(_local->_ip));
	}

	_lsock.sin6_scope_id = 0;
   
	if(local_port_ > 0) {
		_lsock.sin6_port = htons(local_port_);
	} else {
		_lsock.sin6_port = htons(-1);
	}

	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));

	if (::bind(_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) < 0) {
		Close();

		throw SocketException("Binding error");
	}
#endif
}

void DatagramSocket6::ConnectSocket(InetAddress *addr_, int port_)
{
#ifdef _WIN32
#else
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
		if (_timeout > 0) {
			long arg;

			if( (arg = fcntl(_fd, F_GETFL, NULL)) < 0) { 
				throw SocketException("Cannont set non blocking socket");
			}

			arg |= O_NONBLOCK; 

			if( fcntl(_fd, F_SETFL, arg) < 0) { 
				throw SocketException("Cannont set non blocking socket");
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

						r = select(_fd+1, NULL, &wset, NULL, &tv); 

						if (r < 0 && errno != EINTR) { 
							throw SocketException("Connection error");
						} else if (r > 0) { 
							socklen_t len = sizeof(int); 
							int val; 

							if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void*)(&val), &len) < 0) { 
								throw SocketException("Unknown error in getsockopt()");
							} 

							if (val) { 
								throw SocketException("Error in delayed connection");
							}

							break; 
						} else { 
							throw SocketException("Socket connection timeout exception");
						} 
					} while (true); 
				} else { 
					throw SocketException("Unknown error");
				} 
			}
		} else {
			r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
		}

		if (r < 0) {
			throw SocketException("Connection error");
		}
	}
#endif
}

void DatagramSocket6::InitStream(int rbuf_, int wbuf_)
{
#ifdef _WIN32
#else
	if (_stream == false) {
		_is = new SocketInputStream((Connection *)this, (struct sockaddr *)&_server_sock, rbuf_);
		_os = new SocketOutputStream((Connection *)this, (struct sockaddr *)&_server_sock, wbuf_);
	} else {
		_is = new SocketInputStream((Connection *)this, rbuf_);
		_os = new SocketOutputStream((Connection *)this, wbuf_);
	}
#endif
}

/** End */

jsocket_t DatagramSocket6::GetHandler()
{
	return _fd;
}

jio::InputStream * DatagramSocket6::GetInputStream()
{
#ifdef _WIN32
	return NULL;
#else
	return (jio::InputStream *)_is;
#endif
}

jio::OutputStream * DatagramSocket6::GetOutputStream()
{
#ifdef _WIN32
	return NULL;
#else
	return (jio::OutputStream *)_os;
#endif
}

int DatagramSocket6::Receive(char *data_, int size_, int time_)
{
#ifdef _WIN32
	return 0;
#else
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}
	
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
			return DatagramSocket6::Receive(data_, size_, true);
	    }
	}

	return -1;
#endif
}

int DatagramSocket6::Receive(char *data_, int size_, bool block_)
{
#ifdef _WIN32
	return 0;
#else
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}
	
	int n,
		flags = 0,
		length = sizeof(_server_sock);

	if (block_ == false) {
		flags = MSG_DONTWAIT;
	}

	n = ::recvfrom(_fd, data_, size_, flags, (struct sockaddr *)&_server_sock, (socklen_t *)&length);
	
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

		throw jio::IOException("Broken pipe exception");
	}
	*/

	_receive_bytes += n;

	return n;
#endif
}

int DatagramSocket6::Send(const char *data_, int size_, int time_)
{
#ifdef _WIN32
	return 0;
#else
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}
	
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
			return DatagramSocket6::Send(data_, size_);
	    }
	}

	return -1;
#endif
}

int DatagramSocket6::Send(const char *data_, int size_, bool block_)
{
#ifdef _WIN32
	return 0;
#else
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}
	
	int n,
	   	flags = 0;

	if (block_ == false) {
		flags = MSG_NOSIGNAL | MSG_DONTWAIT;
	}

	if (_stream == true) {	
		n = ::send(_fd, data_, size_, flags);
	} else {
		n = ::sendto(_fd, data_, size_, flags, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
	}

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

	_sent_bytes += n;
	
	return n;
#endif
}

void DatagramSocket6::Close()
{
#ifdef _WIN32
#else
	if (_is_closed == true) {
		return;
	}

	if (close(_fd) != 0) {
		throw SocketException("Unknown close exception");
	}
	
	_is_closed = true;
#endif
}

InetAddress * DatagramSocket6::GetInetAddress()
{
#ifdef _WIN32
	return NULL;
#else
	return _address;
#endif
}

int DatagramSocket6::GetLocalPort()
{
#ifdef _WIN32
	return 0;
#else
	return ntohs(_lsock.sin6_port);
#endif
}

int DatagramSocket6::GetPort()
{
#ifdef _WIN32
	return 0;
#else
	return ntohs(_server_sock.sin6_port);
#endif
}

int64_t DatagramSocket6::GetSentBytes()
{
#ifdef _WIN32
	return 0LL;
#else
	return _sent_bytes + _os->GetSentBytes();
#endif
}

int64_t DatagramSocket6::GetReadedBytes()
{
#ifdef _WIN32
	return 0LL;
#else
	return _receive_bytes + _is->GetReadedBytes();
#endif
}

SocketOptions * DatagramSocket6::GetSocketOptions()
{
#ifdef _WIN32
	return NULL;
#else
	return new SocketOptions(_fd, JCT_UDP);
#endif
}

std::string DatagramSocket6::what()
{
	char port[20];
   
	sprintf(port, "%u", GetPort());

	return GetInetAddress()->GetHostName() + ":" + port;
}

}
