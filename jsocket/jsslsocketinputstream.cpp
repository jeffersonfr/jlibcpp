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
#include "jsslsocketinputstream.h"
#include "jsocketexception.h"

namespace jsocket {

SSLSocketInputStream::SSLSocketInputStream(Connection *conn_, bool *is_closed_, void *ssl, int64_t size_):
	jio::InputStream()
{
	jcommon::Object::SetClassName("jsocket::SSLSocketInputStream");
	
#ifdef _WIN32
#else
	_ssl = (SSL *)ssl;
	_connection = conn_;
	_fd = conn_->GetHandler();
	_is_closed = is_closed_;
	_stream = true;
	_buffer_length = size_;
	_current_index = 0;
	_end_index = 0;
	_receive_bytes = 0;
	_blocked = true;
	
	try {
		_buffer = new char[(uint32_t)_buffer_length];
	} catch (std::bad_alloc &e) {
		_buffer = NULL;

		_buffer_length = 0;
		_current_index = 0;
	}
#endif
}

SSLSocketInputStream::~SSLSocketInputStream()
{
#ifdef _WIN32
#else
	if (_buffer != NULL) {
		delete [] _buffer;
	}
#endif
}

int64_t SSLSocketInputStream::Read()
{
#ifdef _WIN32
	return 0LL;
#else
	if ((*_is_closed) == true) {
		throw SocketException("Connection closed exception");
	}
	
	int d = (int)(_end_index - _current_index);

	if (d == 0) {
		int n;

		n = SSL_read(_ssl, _buffer, _buffer_length);
		
		if (n <= 0) {
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
#endif
}

int64_t SSLSocketInputStream::Read(char *data_, int64_t data_length_)
{
#ifdef _WIN32
	return 0LL;
#else
	if ((*_is_closed) == true) {
		throw SocketException("Connection closed exception");
	}
	
	// retorna no maximo o tamanho do buffer em bytes
	
	int64_t d, 
		 r;
	
	d = _end_index - _current_index;

	if (d == 0LL) {
		int n;

		n = SSL_read(_ssl, data_, _buffer_length);
		
		if (n <= 0) {
			return -1LL;
		}
			
		_current_index = 0;
		_end_index = n;
		_receive_bytes += n;

		d = n;
	}
	
	if (data_length_ <= d) {
		memcpy(data_, (_buffer + _current_index), (size_t)data_length_);
		_current_index += data_length_;
		r = data_length_;
	} else {
		memcpy(data_, (_buffer + _current_index), (size_t)d);
		_current_index += d;
		r = d;
	}
	
	if (_current_index >= _end_index) {
		_current_index = _end_index = 0;
	}

	return (int64_t)r;
#endif
}

bool SSLSocketInputStream::IsEmpty()
{
#ifdef _WIN32
	return false;
#else
	return (_current_index == _end_index);
#endif
}

int64_t SSLSocketInputStream::Available()
{
#ifdef _WIN32
	return 0LL;
#else
	return (int64_t)_current_index;
#endif
}

int64_t SSLSocketInputStream::GetReadedBytes()
{
#ifdef _WIN32
	return 0LL;
#else
	return (int64_t)_receive_bytes;
#endif
}

void SSLSocketInputStream::Close()
{
#ifdef _WIN32
#else
	_connection->Close();
#endif
}

void SSLSocketInputStream::Reset()
{
#ifdef _WIN32
#else
	_current_index = _end_index = 0;
#endif
}

int64_t SSLSocketInputStream::GetSize()
{
	return 0LL;
}

int64_t SSLSocketInputStream::GetPosition()
{
	return 0LL;
}

void SSLSocketInputStream::Skip(int64_t skip)
{
	// DO:: nothing
}

}
