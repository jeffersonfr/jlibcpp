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
#include "jnetwork/jsocketinputstream.h"
#include "jexception/jconnectionexception.h"

#include <string.h>

namespace jnetwork {

SocketInputStream::SocketInputStream(Connection *connection_, int64_t size_):
	jio::InputStream()
{
	jcommon::Object::SetClassName("jnetwork::SocketInputStream");
	
	_connection = connection_;
	_stream = true;
	_buffer_length = size_;
	_current_index = 0LL;
	_end_index = 0LL;
	_receive_bytes = 0LL;
	_blocked = true;
	
	try {
		_buffer = new char[(int)_buffer_length];
	} catch (std::bad_alloc &) {
		_buffer = nullptr;

		_buffer_length = 0LL;
		_current_index = 0LL;
	}
}

SocketInputStream::SocketInputStream(Connection *connection_, struct sockaddr *address, int64_t size_):
	jio::InputStream()
{
	jcommon::Object::SetClassName("jnetwork::SocketInputStream");
	
	_connection = connection_;
	_stream = false;
	_buffer_length = size_;
	_current_index = 0LL;
	_end_index = 0LL;
	_receive_bytes = 0LL;
	_address = address;
	_blocked = true;
	
	try {
		_buffer = new char[(int)_buffer_length];
	} catch (std::bad_alloc &) {
		_buffer = nullptr;

		_buffer_length = 0LL;
		_current_index = 0LL;
	}
}

SocketInputStream::~SocketInputStream()
{
	if (_buffer != nullptr) {
		delete [] _buffer;
    _buffer = nullptr;
	}
}

int64_t SocketInputStream::Read()
{
	if (_connection->IsClosed() == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	int flags = 0,
		d = (int)(_end_index - _current_index);

	if (d == 0) {
		int n,
			length = sizeof(*_address);

		if (_stream == true) {
			n = ::recv(_connection->_fd, _buffer, (size_t)_buffer_length, flags);
		} else {
			n = ::recvfrom(_connection->_fd, _buffer, (size_t)_buffer_length, flags, _address, (socklen_t *)&length);
		}
		
		if (n <= 0) {
			if (n == 0) {
				Close();
			}

			return -1LL;
		}
			
		_current_index = 0;
		_end_index = n;
		_receive_bytes += n;
	}
	
	uint8_t c;
	
	c = _buffer[_current_index];
	
	if (++_current_index >= _end_index) {
		_current_index = _end_index = 0;
	}
	
	return (int64_t)c;
}

int64_t SocketInputStream::Read(char *data_, int64_t data_length_)
{
	if (_connection->IsClosed() == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	// retorna no maximo o tamanho do buffer em bytes
	
	int flags = 0LL;
	int d, 
		r;
	
	d = (int)(_end_index - _current_index);

	if (d == 0) {
		int n,
			length = sizeof(*_address);

		if (_stream == true) {
			n = ::recv(_connection->_fd, _buffer, (size_t)_buffer_length, flags);
		} else {
			n = ::recvfrom(_connection->_fd, _buffer, (size_t)_buffer_length, flags, _address, (socklen_t *)&length);
		}
	
		if (n <= 0) {
			if (n == 0) {
				Close();
			}

			return -1LL;
		}
			
		_current_index = 0;
		_end_index = n;
		_receive_bytes += n;

		d = n;
	}
	
	if (data_length_ <= d) {
		memcpy(data_, (_buffer + (size_t)_current_index), (size_t)data_length_);
		_current_index += data_length_;
		r = (int)data_length_;
	} else {
		memcpy(data_, (_buffer + (size_t)_current_index), d);
		_current_index += d;
		r = d;
	}
	
	if (_current_index >= _end_index) {
		_current_index = _end_index = 0;
	}

	return (int64_t)r;
}

bool SocketInputStream::IsEmpty()
{
	return (_current_index == _end_index);
}

int64_t SocketInputStream::Available()
{
	return (int64_t)_current_index;
}

int64_t SocketInputStream::GetReadedBytes()
{
	return (int64_t)_receive_bytes;
}

void SocketInputStream::Close()
{
	_connection->Close();
}

bool SocketInputStream::IsClosed()
{
	return _connection->IsClosed();
}

void SocketInputStream::Reset()
{
	_current_index = _end_index = 0;
}

int64_t SocketInputStream::GetSize()
{
	return 0LL;
}

int64_t SocketInputStream::GetPosition()
{
	return 0LL;
}

void SocketInputStream::Skip(int64_t skip)
{
	// DO:: nothing
}

}
