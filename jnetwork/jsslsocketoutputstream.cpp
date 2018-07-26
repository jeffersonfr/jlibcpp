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
#include "jnetwork/jsslsocketoutputstream.h"
#include "jexception/jconnectionexception.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <string.h>

namespace jnetwork {

SSLSocketOutputStream::SSLSocketOutputStream(Connection *connection_, void *ssl, int64_t size_):
	jio::OutputStream()
{
	jcommon::Object::SetClassName("jnetwork::SSLSocketOutputStream");
	
	_ssl = (SSL *)ssl;
	_connection = connection_;
	_buffer_length = size_;
	_current_index = 0;
	_sent_bytes = 0;
	_stream = true;
	_blocked = true;
	
	try {
		_buffer = new char[(uint32_t)_buffer_length];
	} catch (std::bad_alloc &e) {
		_buffer = NULL;

		_buffer_length = 0;
		_current_index = 0;
	}
}

SSLSocketOutputStream::~SSLSocketOutputStream()
{
	if (_buffer != NULL) {
		delete [] _buffer;
	}
}

int64_t SSLSocketOutputStream::Available()
{
	return 0LL;
}

int64_t SSLSocketOutputStream::Write(int64_t c_)
{
	_buffer[_current_index++] = (char)c_;

	if (_current_index == _buffer_length) {
		return Flush();
	}

	return 0;
}

int64_t SSLSocketOutputStream::Write(const char *data_, int64_t data_length_)
{
	int64_t l = data_length_, size; 
	
	while (l > 0LL) {
		size = (_buffer_length - _current_index);
		
		if (l < size) {
			memcpy((_buffer + _current_index), (data_ + data_length_ - l), (size_t)l);
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

bool SSLSocketOutputStream::IsEmpty()
{
	return (_current_index == 0);
}

int64_t SSLSocketOutputStream::GetAvailable()
{
	return _current_index;
}

int64_t SSLSocketOutputStream::GetSentBytes()
{
	return _sent_bytes;
}

int64_t SSLSocketOutputStream::Flush()
{
	if (_connection->IsClosed() == true) {
		throw jexception::ConnectionException("Connection closed exception");
	}
	
	if (_current_index == 0) {
		return 0LL;
	}

	int n;

	n = SSL_write(_ssl, _buffer, _current_index);

	_current_index = 0;

	if (n < 0) {
		return -1LL;
	}

	_sent_bytes += n;

	return (int64_t)n;
}

int64_t SSLSocketOutputStream::GetSize()
{
	return 0LL;
}

void SSLSocketOutputStream::Seek(int64_t index)
{
	// DO:: nothing
}

void SSLSocketOutputStream::Close()
{
	_connection->Close();
}

bool SSLSocketOutputStream::IsClosed()
{
	return _connection->IsClosed();
}

}
