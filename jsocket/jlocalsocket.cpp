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
#include "jlocalsocket.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jioexception.h"

namespace jsocket {

LocalSocket::LocalSocket(std::string file, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jsocket::Socket");

	_file = file;

#ifdef _WIN32
	throw jcommon::SocketException("Named socket unsupported.");
#endif

	_is = NULL;
	_os = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	CreateSocket();
	ConnectSocket();
	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

LocalSocket::~LocalSocket()
{
	try {
		Close();
	} catch (...) {
	}

	if (_is != NULL) {
		delete _is;
		_is = NULL;
	}

	if (_os != NULL) {
		delete _os;
		_os = NULL;
	}
}

/** Private */

LocalSocket::LocalSocket(jsocket_t handler_, std::string file_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(JCT_TCP)
{
	jcommon::Object::SetClassName("jsocket::Socket");

	_fd = handler_;
	_file = file_;

	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

void LocalSocket::CreateSocket()
{
#ifdef _WIN32
#else
	_fd = socket (PF_UNIX, SOCK_STREAM, PF_UNSPEC);

	if (_fd < 0) {
		throw SocketException("Socket handling error");
	}
#endif
}

void LocalSocket::ConnectSocket()
{
#ifdef _WIN32
#else
	int length = sizeof(_address.sun_path)-1;

	_address.sun_family = AF_UNIX;
	strncpy(_address.sun_path, _file.c_str(), length);
	
	int r,
			address_length = sizeof(_address.sun_family) + strnlen(_address.sun_path, length);

	if (_timeout > 0) {
		int opt = 1;

		ioctl(_fd, FIONBIO, &opt);

		r = connect(_fd, (struct sockaddr *)&_address, address_length);

		if (errno != EINPROGRESS) {
			throw SocketException("Connection error");
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
					throw SocketException("Connection error");
				}

				shutdown(_fd, SHUT_RDWR);

				if (r == 0) {
					throw SocketException("Socket connection timeout exception");
				} else if (r < 0) {
					throw SocketException("Connection error");
				}
			}

			int optlen = sizeof(r);

			getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void *)&r, (socklen_t *)&optlen);

			if (r != 0) {
				throw SocketException("Unknown error");
			}
		}

		opt = 0;

		if (ioctl(_fd, FIONBIO, &opt) < 0) {
			throw SocketException("Connection error");
		}
	} else {
		r = connect(_fd, (struct sockaddr *)&_address, sizeof(_address));
	}

	if (r < 0) {
		throw SocketException("Connection error");
	}
#endif
}

void LocalSocket::InitStreams(int64_t rbuf_, int64_t wbuf_)
{
	_is = new SocketInputStream((Connection *)this, &_is_closed, rbuf_);
	_os = new SocketOutputStream((Connection *)this, &_is_closed, wbuf_);
}

/** End */

jsocket_t LocalSocket::GetHandler()
{
#ifdef _WIN32
	return -1;
#else
	return _fd;
#endif
}

std::string LocalSocket::GetLocalFile()
{
	return _file;
}

int LocalSocket::Send(const char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

#ifdef _WIN32
	return -1;
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
			return LocalSocket::Send(data_, size_);
		}
	}
#endif

	return -1;
}

int LocalSocket::Send(const char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

#ifdef _WIN32
	return -1;
#else
	int flags;

	if (block_ == true) {
		flags = MSG_NOSIGNAL;
	} else {
		flags = MSG_NOSIGNAL | MSG_DONTWAIT;
	}

	int n = ::send(_fd, data_, size_, flags);

	if (n < 0) {
		if (errno == EAGAIN) {
			if (block_ == true) {
				throw SocketTimeoutException("Socket output timeout error");
			} else {
				// INFO:: non-blocking socket, no data read
				n = 0;
			}
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

int LocalSocket::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

#ifdef _WIN32
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
			return LocalSocket::Receive(data_, size_);
		}
	}
#endif
	
	return -1;
}

int LocalSocket::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection closed exception");
	}

#ifdef _WIN32
#else
	int flags = 0;

	if (block_ == false) {
		flags = MSG_DONTWAIT;
	}

	int n = ::recv(_fd, data_, size_, flags);

	if (n < 0) {
		if (errno == EAGAIN) {
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
		
		throw SocketException("Broken pipe exception");
	}

	_receive_bytes += n;

	return n;
#endif
}

void LocalSocket::Close()
{
	if (_is_closed == true) {
		return;
	}

#ifdef _WIN32
#else
	if (close(_fd) != 0) {
		throw SocketException("Unknown close exception");
	}
	
	unlink(_file.c_str());
#endif
		
	_is_closed = true;
}

jio::InputStream * LocalSocket::GetInputStream()
{
	return (jio::InputStream *)_is;
}

jio::OutputStream * LocalSocket::GetOutputStream()
{
	return (jio::OutputStream *)_os;
}

int64_t LocalSocket::GetSentBytes()
{
	return _sent_bytes + _os->GetSentBytes();
}

int64_t LocalSocket::GetReadedBytes()
{
	return _receive_bytes + _is->GetReadedBytes();
}

SocketOptions * LocalSocket::GetSocketOptions()
{
	return new SocketOptions(_fd, JCT_TCP);
}

}
