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
#include "jconnectionpipe.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jsocketstreamexception.h"

namespace jsocket {

int ConnectionPipe::_used_port = 1024;

ConnectionPipe::ConnectionPipe(Connection *conn, jconnection_pipe_t type_, int size_pipe_, int timeout_, bool stream_):
	jsocket::Connection(conn->GetType())
{
	// jcommon::Object::SetClassName("jsocket::ConnectionPipe");
	Connection::SetClassName("jsocket::ConnectionPipe");
	
	_connection = conn;
	_stream = stream_;
	_is_closed = true;
	_timeout = timeout_;
	_pipe_type = type_;
	_current_send = 0;
	_size_pipe = size_pipe_;

	int r;
	
#ifdef _WIN32
	_pipe = new HANDLE[2];

	r = CreatePipe(&_pipe[0], &_pipe[1], 0, size_pipe_);
#else
	r = pipe(_pipe);
#endif
	
	if (r <= 0) {
		_is_closed = true;
	}

	_is_closed = false;
}

ConnectionPipe::~ConnectionPipe()
{
	try {
		Close();
	} catch (...) {
	}

	if ((void *)_pipe != NULL) {
		delete _pipe;
	}
}

int ConnectionPipe::Receive(char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
#ifdef _WIN32
	return ConnectionPipe::Receive(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _pipe[0];
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Send timeout exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket send timeout exception");
	} else {
	    if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLOUT)) {
			return ConnectionPipe::Receive(data_, size_);
	    }
	}
#endif

	return -1;
}

int ConnectionPipe::Receive(char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	int r = 0;

#ifdef _WIN32
	ReadFile(_pipe[0], data_, size_, (DWORD *)&r, 0);

	if (r <= 0) {
		throw SocketStreamException("Socket broken pipe");
	}
#else
	char *c = data_;
	
	r = read(_pipe[0], c, size_);
	
	if (r < 0) {
		if (errno == EAGAIN) {
			return -1;
		} else {
			throw SocketStreamException("Socket broken pipe");
		}
	}
#endif
	
	_current_send -= r;
	
	return r;
}

int ConnectionPipe::Send(const char *data_, int size_, int time_)
{
	if (_is_closed == true) {
		throw SocketException("Connection was closed");
	}

#ifdef _WIN32
	return ConnectionPipe::Send(data_, size_);
#else
	struct pollfd ufds[1];

	ufds[0].fd = _pipe[1];
	ufds[0].events = POLLOUT | POLLWRBAND;

	int rv = poll(ufds, 1, time_);

	if (rv == -1) {
		throw SocketException("Send timeout exception");
	} else if (rv == 0) {
		throw SocketTimeoutException("Socket send timeout exception");
	} else {
	    if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLOUT)) {
			return ConnectionPipe::Send(data_, size_);
	    }
	}
#endif

	return -1;
}

int ConnectionPipe::Send(const char *data_, int size_, bool block_)
{
	if (_is_closed == true) {
		throw SocketException("Connection is closed");
	}
	
	int n = 0;

#ifdef _WIN32
	WriteFile(_pipe[1], data_, size_, (DWORD *)&n, 0);
#else
	n = write(_pipe[1], data_, size_);
#endif
	
	if (n < 0) {
		throw SocketStreamException("Socket broken pipe");
	}
	
	return n;
}

void ConnectionPipe::Close()
{
	if (_is_closed == false) {
#ifdef _WIN32
		CloseHandle(_pipe[0]);
		CloseHandle(_pipe[1]);
#else
		close(_pipe[0]);
		close(_pipe[1]);
#endif
	}
	
	_connection->Close();
	
	_is_closed = true;
}

#ifdef _WIN32
SOCKET ConnectionPipe::GetHandler()
#else
int ConnectionPipe::GetHandler()
#endif
{
	return _connection->GetHandler();
}

jio::InputStream * ConnectionPipe::GetInputStream()
{
	return NULL;
}

jio::OutputStream * ConnectionPipe::GetOutputStream()
{
	return NULL;
}

int64_t ConnectionPipe::GetSentBytes()
{
	return _connection->GetSentBytes();
}

int64_t ConnectionPipe::GetReadedBytes()
{
	return _connection->GetReadedBytes();
}

// thread members

void ConnectionPipe::Run()
{
	if (_pipe_type == RECEIVER_PIPE) {
		main_pipe_receiver();
	} else {
		main_pipe_sender();
	}
}

void ConnectionPipe::main_pipe_receiver()
{
#ifdef _WIN32
	int n,
		length = 0,
		size_buffer = _size_pipe;
	HANDLE pipe = _pipe[0];
	char *c,
		 *buffer = new char[size_buffer];

	_current_send = 0;
#else
	int n,
		pipe = _pipe[1],
		size_buffer = PIPE_BUF;
	char *c,
		 buffer[size_buffer];

	_current_send = 0;
#endif
	
	// receive pipe
	
	while (true) {
		c = buffer;
		
		try {
			n = _connection->Receive(buffer, size_buffer);
		} catch (...) {
#ifdef _WIN32
			delete buffer;
			buffer = NULL;
#endif

			return;
		}

#ifdef _WIN32
		int n;

		WriteFile(pipe, buffer, n, (DWORD *)&n, 0);
#else
		n = write(pipe, buffer, n);
#endif
		
		_current_send += n;
		_receive_bytes += n;
	}
}

void ConnectionPipe::main_pipe_sender()
{
#ifdef _WIN32
	int n,
		r = 0,
		count = 0,
		size_buffer = _size_pipe;
	HANDLE pipe = _pipe[0];
	char *c,
		 *buffer = new char[size_buffer];
	bool stream = _stream;

	while (true) {
		c = buffer;
		count = 0;
		
		while (count < size_buffer) {
			ReadFile(pipe, (c + count), size_buffer - count, (DWORD *)&r, 0);

			if (r <= 0) {
				return;
			}
		}
#else
	int n,
		r = 0,
		count = 0,
		pipe = _pipe[0],
		size_buffer = _size_pipe;
	char *c,
		 buffer[size_buffer];

	while (true) {
		c = buffer;
		count = 0;
		
		while (count < size_buffer) {
			count += r = read(pipe, (c + count), size_buffer-count);
		
			if (r < 0) {
				return;
			}
		}
#endif
		
		// send packet

		try {
			n = _connection->Send(buffer, size_buffer); // MSG_NOSIGNAL
		} catch (...) {
#ifdef _WIN32
			delete buffer;
			buffer = NULL;
#endif

			return;
		}

		_sent_bytes += n;
	}
}

}
