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
#include "jsocketstreamexception.h"

namespace jsocket {

LocalSocket::LocalSocket(std::string file, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::Socket");

	_file = file;

#ifdef _WIN32
	throw jcommon::SocketException("Unamed socket unsupported.");
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

LocalSocket::LocalSocket(int handler_, std::string file_, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
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
	_fd = socket (PF_UNIX, SOCK_STREAM, 0); // IPPROTO_TCP);

	if (_fd < 0) {
		throw SocketException("Create socket error");
	}
#endif
}

void LocalSocket::ConnectSocket()
{
#ifdef _WIN32
#else
	_address.sun_family = PF_UNIX;
	strncpy(_address.sun_path, _file.c_str(), 255);
	
	int r,
			address_length = sizeof(_address.sun_family) + strnlen(_address.sun_path, 255);

	unlink(_file.c_str());

	if (_timeout > 0) {
		int opt = 1;

		ioctl(_fd, FIONBIO, &opt);

		r = connect(_fd, (struct sockaddr *)&_address, address_length);

		if (errno != EINPROGRESS) {
			opt = 0;

			if (ioctl(_fd, FIONBIO, &opt) < 0) {
				throw SocketException("Socket non-blocking error");
			}

			throw SocketException("Connect socket error");
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
					throw SocketException("Socket non-blocking error");
				}

				shutdown(_fd, SHUT_RDWR);

				if (r == 0) {
					throw SocketException("Connect timeout error");
				} else if (r < 0) {
					throw SocketException("Connect socket error");
				}
			}

			int optlen = sizeof(r);

			getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void *)&r, (socklen_t *)&optlen);

			if (r != 0) {
				throw SocketException("Can't connect socket");
			}
		}

		opt = 0;

		if (ioctl(_fd, FIONBIO, &opt) < 0) {
			throw SocketException("Socket non-blocking error");
		}
	} else {
		r = connect(_fd, (struct sockaddr *)&_address, sizeof(_address));
	}

	if (r < 0) {
		throw SocketException("Connect socket error");
	}
#endif
}

void LocalSocket::InitStreams(int64_t rbuf_, int64_t wbuf_)
{
	_is = new SocketInputStream((Connection *)this, &_is_closed, rbuf_);
	_os = new SocketOutputStream((Connection *)this, &_is_closed, wbuf_);
}

/** End */

#ifdef _WIN32
SOCKET LocalSocket::GetHandler()
#else
int LocalSocket::GetHandler()
#endif
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
		throw SocketException("Connection was closed");
	}

#ifdef _WIN32
	return -1;
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
			return LocalSocket::Send(data_, size_);
		}
	}
#endif

	return -1;
}

int LocalSocket::Send(const char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
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

	if (n < 0 && errno == EAGAIN) {
		if (block_ == false) {
			throw SocketStreamException("Socket buffer is empty");
		} else {
			throw SocketTimeoutException("Socket send timeout exception");
		}
	} else if (n < 0) {
		throw SocketStreamException("Send socket error");
	}

	_sent_bytes += n;

	return n;
#endif
}

int LocalSocket::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}

#ifdef _WIN32
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
			return LocalSocket::Receive(data_, size_);
		}
	}
#endif
	
	return -1;
}

int LocalSocket::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}

#ifdef _WIN32
#else
	int flags = 0;

	if (block_ == false) {
		flags = MSG_DONTWAIT;
	}

	int n = ::recv(_fd, data_, size_, flags);

	if (n < 0 && errno == EAGAIN) {
		if (block_ == false) {
			throw SocketStreamException("Socket buffer is empty");
		} else {
			throw SocketTimeoutException("Socket receive timeout exception");
		}
	} else if (n < 0) {
		throw SocketStreamException("Read socket error");
	} else if (n == 0) {
		//throw SocketException("Peer has shutdown");
		return -1;
	}

	_receive_bytes += n;

	return n;
#endif
}

void LocalSocket::Close()
{
#ifdef _WIN32
#else
	if (_is_closed == false) {
		_is_closed = true;

		if (close(_fd) != 0) {
			throw SocketException("Close socket error");
		}
	}
	
	unlink(_file.c_str());
#endif
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

SocketOption * LocalSocket::GetSocketOption()
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}

	return new SocketOption(_fd, TCP_SOCKET);
}

}
