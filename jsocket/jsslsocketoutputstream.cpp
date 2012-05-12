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
#include "jsslsocketoutputstream.h"
#include "jsocketexception.h"

namespace jsocket {

SSLSocketOutputStream::SSLSocketOutputStream(Connection *conn_, bool *is_closed_, void *ssl, int64_t size_):
	jio::OutputStream()
{
	jcommon::Object::SetClassName("jsocket::SSLSocketOutputStream");
	
#ifdef _WIN32
#else
	_ssl = (SSL *)ssl;
	_connection = conn_;
	_fd = conn_->GetHandler();
	_is_closed = is_closed_;
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
#endif
}

SSLSocketOutputStream::~SSLSocketOutputStream()
{
#ifdef _WIN32
#else
	if (_buffer != NULL) {
		delete [] _buffer;
	}
#endif
}

int64_t SSLSocketOutputStream::Available()
{
	return 0LL;
}

int64_t SSLSocketOutputStream::Write(int64_t c_)
{
#ifdef _WIN32
#else
	_buffer[_current_index++] = (char)c_;

	if (_current_index == _buffer_length) {
		return Flush();
	}
#endif

	return 0;
}

int64_t SSLSocketOutputStream::Write(const char *data_, int64_t data_length_)
{
#ifdef _WIN32
	return 0LL;
#else
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
#endif
}

bool SSLSocketOutputStream::IsEmpty()
{
#ifdef _WIN32
	return false;
#else
	return (_current_index == 0);
#endif
}

int64_t SSLSocketOutputStream::GetAvailable()
{
#ifdef _WIN32
	return 0LL;
#else
	return _current_index;
#endif
}

int64_t SSLSocketOutputStream::GetSentBytes()
{
#ifdef _WIN32
	return 0LL;
#else
	return _sent_bytes;
#endif
}

int64_t SSLSocketOutputStream::Flush()
{
#ifdef _WIN32
	return 0LL;
#else
	if ((*_is_closed) == true) {
		throw SocketException("Connection closed exception");
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
#endif
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
#ifdef _WIN32
#else
	_connection->Close();
#endif
}

}
