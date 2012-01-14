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
#include "jsocketoutputstream.h"
#include "jsocketexception.h"

namespace jsocket {

SocketOutputStream::SocketOutputStream(Connection *conn_, bool *is_closed_, int64_t size_):
	jio::OutputStream()
{
	jcommon::Object::SetClassName("jsocket::SocketOutputStream");
	
	_connection = conn_;
	_fd = conn_->GetHandler();
	_is_closed = is_closed_;
	_buffer_length = size_;
	_current_index = 0;
	_sent_bytes = 0;
	_stream = true;
	_blocked = true;
	
	try {
		_buffer = new char[(int)_buffer_length];
	} catch (std::bad_alloc &) {
		_buffer = NULL;

		_buffer_length = 0;
		_current_index = 0;
	}
}

SocketOutputStream::SocketOutputStream(Connection *conn_, bool *is_closed_, struct sockaddr *address, int64_t size_):
	jio::OutputStream()
{
	jcommon::Object::SetClassName("jsocket::SocketOutputStream");
	
	_connection = conn_;
	_fd = conn_->GetHandler();
	_is_closed = is_closed_;
	_buffer_length = size_;
	_current_index = 0LL;
	_sent_bytes = 0;
	_stream = false;
	_blocked = true;
	_address = address;

	try {
		_buffer = new char[(int)_buffer_length];
	} catch (std::bad_alloc &) {
		_buffer = NULL;

		_buffer_length = 0;
		_current_index = 0;
	}
}

SocketOutputStream::~SocketOutputStream()
{
	if (_buffer != NULL) {
		delete [] _buffer;
	}
}

int64_t SocketOutputStream::Available()
{
	return 0LL;
}

int64_t SocketOutputStream::Write(int64_t c_)
{
	_buffer[_current_index++] = (char)c_;

	if (_current_index == _buffer_length) {
		return Flush();
	}

	return 0LL;
}

int64_t SocketOutputStream::Write(const char *data_, int64_t data_length_)
{
	int64_t l = data_length_, 
		size; 
	
	while (l > 0LL) {
		size = (_buffer_length - _current_index);
		
		if (l < size) {
			memcpy((_buffer + (size_t)_current_index), (data_ + (size_t)(data_length_ - l)), (size_t)l);
			_current_index += l;
			
			break;
		} else {
			memcpy((_buffer + _current_index), (data_ + data_length_ - l), (size_t)size);

			l = l - size;
			_current_index = _buffer_length;

			if (Flush() == -1LL) {
				return -1LL;
			}
		}
	}

	return (int64_t)(data_length_ - l);
}

bool SocketOutputStream::IsEmpty()
{
	return (_current_index == 0);
}

int64_t SocketOutputStream::GetAvailable()
{
	return _current_index;
}

int64_t SocketOutputStream::GetSentBytes()
{
	return _sent_bytes;
}

int64_t SocketOutputStream::Flush()
{
	if ((*_is_closed) == true) {
		throw SocketException("Connection closed exception");
	}
	
	if (_current_index == 0) {
		return 0LL;
	}

	int n,
	   	flags;

#ifdef _WIN32
		flags = 0;
#else
		flags = MSG_NOSIGNAL;
#endif

	if (_stream == true) {
		n = ::send(_fd, _buffer, (size_t)_current_index, flags);
	} else {
		n = ::sendto(_fd, _buffer, (size_t)_current_index, flags, _address, sizeof(*_address));
	}

	_current_index = 0;

#ifdef _WIN32
	if (n == SOCKET_ERROR) {
#else
	if (n < 0) {
		if (errno == EPIPE || errno == ECONNRESET) {
			Close();
		}

#endif
		return -1LL;
	}

	_sent_bytes += n;

	return (int64_t)n;
}

int64_t SocketOutputStream::GetSize()
{
	return 0LL;
}

void SocketOutputStream::Seek(int64_t index)
{
	// DO:: nothing
}

void SocketOutputStream::Close()
{
	_connection->Close();
}

}
