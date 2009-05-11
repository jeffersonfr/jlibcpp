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
#include "jsslsocketoutputstream.h"
#include "jsocketexception.h"

namespace jsocket {

SSLSocketOutputStream::SSLSocketOutputStream(Connection *conn_, bool *is_closed_, SSL *ssl, int size_):
	jio::OutputStream()
{
	jcommon::Object::SetClassName("jsocket::SSLSocketOutputStream");
	
	_ssl = ssl;
	_connection = conn_;
	_fd = conn_->GetHandler();
	_is_closed = is_closed_;
	_buffer_length = size_;
	_current_index = 0;
	_sent_bytes = 0;
	_stream = true;
	_blocked = true;
	
	try {
		_buffer = new char[_buffer_length];
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

long long SSLSocketOutputStream::Available()
{
	return 0LL;
}

int SSLSocketOutputStream::Write(int c_)
{
	_buffer[_current_index++] = c_;

	if (_current_index == _buffer_length) {
		return Flush();
	}

	return 0;
}

long long SSLSocketOutputStream::Write(const char *data_, long long data_length_)
{
	long long l = data_length_, size; 
	
	while (l > 0LL) {
		size = (_buffer_length - _current_index);
		
		if (l < size) {
			memcpy((_buffer + _current_index), (data_ + data_length_ - l), l);
			_current_index += l;
			
			break;
		} else {
			memcpy((_buffer + _current_index), (data_ + data_length_ - l), size);

			l = l - size;
			_current_index = _buffer_length;

			if (Flush() == -1LL) {
				return -1LL;
			}
		}
	}

	return (long long)(data_length_ - l);
}

bool SSLSocketOutputStream::IsEmpty()
{
	return (_current_index == 0);
}

int SSLSocketOutputStream::GetAvailable()
{
	return _current_index;
}

long long SSLSocketOutputStream::GetSentBytes()
{
	return _sent_bytes;
}

long long SSLSocketOutputStream::Flush()
{
	if ((*_is_closed) == true) {
		throw SocketException("Connection was closed");
	}
	
	if (_current_index == 0) {
		return 0LL;
	}

	int n;

#ifdef _WIN32
	n = ::send(_fd, _buffer, _current_index, flags);
#else
	n = SSL_write(_ssl, _buffer, _current_index);
#endif

	_current_index = 0;

#ifdef _WIN32
	if (n == SOCKET_ERROR) {
#else
	if (n < 0) {
#endif
		return -1LL;
	}

	_sent_bytes += n;

	return (long long)n;
}

long long SSLSocketOutputStream::GetSize()
{
	return 0LL;
}

void SSLSocketOutputStream::Seek(long long index)
{
	// DO:: nothing
}

void SSLSocketOutputStream::Close()
{
	_connection->Close();
}

}
