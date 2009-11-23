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
#include "jsslsocketinputstream.h"
#include "jsocketexception.h"

namespace jsocket {

SSLSocketInputStream::SSLSocketInputStream(Connection *conn_, bool *is_closed_, SSL *ssl, int size_):
	jio::InputStream()
{
	jcommon::Object::SetClassName("jsocket::SSLSocketInputStream");
	
	_ssl = ssl;
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
		_buffer = new char[_buffer_length];
	} catch (std::bad_alloc &e) {
		_buffer = NULL;

		_buffer_length = 0;
		_current_index = 0;
	}
}

SSLSocketInputStream::~SSLSocketInputStream()
{
	if (_buffer != NULL) {
		delete [] _buffer;
	}
}

int SSLSocketInputStream::Read()
{
	if ((*_is_closed) == true) {
		throw SocketException("Connection is closed");
	}
	
	int d = _end_index - _current_index;

	if (d == 0) {
		int n;

#ifdef _WIN32
		n = ::recv(_fd, _buffer, _buffer_length, flags);
#else
		n = SSL_read(_ssl, _buffer, _buffer_length);
#endif
		
#ifdef _WIN32
		if (n == SOCKET_ERROR || n == 0) {
#else 
		if (n <= 0) {
#endif
			// throw SocketException("Read socket error !");
			return -1;
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
	
	return c;
}

long long SSLSocketInputStream::Read(char *data_, long long data_length_)
{
	if ((*_is_closed) == true) {
		throw SocketException("Connection is closed");
	}
	
	// retorna no maximo o tamanho do buffer em bytes
	
	long long d, 
		 r;
	
	d = _end_index - _current_index;

	if (d == 0LL) {
		int n;

#ifdef _WIN32
		n = ::recv(_fd, _buffer, _buffer_length, flags);
#else 
		n = SSL_read(_ssl, data_, _buffer_length);
#endif
		
#ifdef _WIN32
		if (n == SOCKET_ERROR) {
#else 
		if (n <= 0) {
#endif
			// throw SocketException("Read socket error !");
			return -1LL;
		}
			
		_current_index = 0;
		_end_index = n;
		_receive_bytes += n;

		d = n;
	}
	
	if (data_length_ <= d) {
		memcpy(data_, (_buffer + _current_index), data_length_);
		_current_index += data_length_;
		r = data_length_;
	} else {
		memcpy(data_, (_buffer + _current_index), d);
		_current_index += d;
		r = d;
	}
	
	if (_current_index >= _end_index) {
		_current_index = _end_index = 0;
	}

	return (long long)r;
}

bool SSLSocketInputStream::IsEmpty()
{
	return (_current_index == _end_index);
}

long long SSLSocketInputStream::Available()
{
	return (long long)_current_index;
}

long long SSLSocketInputStream::GetReceiveBytes()
{
	return (long long)_receive_bytes;
}

void SSLSocketInputStream::Close()
{
	_connection->Close();
}

void SSLSocketInputStream::Reset()
{
	_current_index = _end_index = 0;
}

long long SSLSocketInputStream::GetSize()
{
	return 0LL;
}

long long SSLSocketInputStream::GetPosition()
{
	return 0LL;
}

void SSLSocketInputStream::Skip(long long skip)
{
	// DO:: nothing
}

}
