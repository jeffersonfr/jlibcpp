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
#include "jnetwork/jsslsocketinputstream.h"
#include "jexception/jconnectionexception.h"

#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>

namespace jnetwork {

SSLSocketInputStream::SSLSocketInputStream(Connection *connection_, void *ssl, int64_t size_):
	jio::InputStream()
{
	jcommon::Object::SetClassName("jnetwork::SSLSocketInputStream");
	
	_ssl = (SSL *)ssl;
	_connection = connection_;
	_stream = true;
	_buffer_length = size_;
	_current_index = 0;
	_end_index = 0;
	_receive_bytes = 0;
	_blocked = true;
	
	try {
		_buffer = new char[(uint32_t)_buffer_length];
	} catch (std::bad_alloc &e) {
		_buffer = nullptr;

		_buffer_length = 0;
		_current_index = 0;
	}
}

SSLSocketInputStream::~SSLSocketInputStream()
{
	if (_buffer != nullptr) {
		delete [] _buffer;
	}
}

int64_t SSLSocketInputStream::Read()
{
	if (_connection->IsClosed() == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	int d = (int)(_end_index - _current_index);

	if (d == 0) {
		int n;

		n = SSL_read(_ssl, _buffer, _buffer_length);
		
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

int64_t SSLSocketInputStream::Read(char *data_, int64_t data_length_)
{
	if (_connection->IsClosed() == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	// retorna no maximo o tamanho do buffer em bytes
	
	int64_t d, 
		 r;
	
	d = _end_index - _current_index;

	if (d == 0LL) {
		int n;

		n = SSL_read(_ssl, _buffer, _buffer_length);
		
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
}

bool SSLSocketInputStream::IsEmpty()
{
	return (_current_index == _end_index);
}

int64_t SSLSocketInputStream::Available()
{
	return (int64_t)_current_index;
}

int64_t SSLSocketInputStream::GetReadedBytes()
{
	return (int64_t)_receive_bytes;
}

void SSLSocketInputStream::Close()
{
	_connection->Close();
}

bool SSLSocketInputStream::IsClosed()
{
	return _connection->IsClosed();
}

void SSLSocketInputStream::Reset()
{
	_current_index = _end_index = 0;
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
