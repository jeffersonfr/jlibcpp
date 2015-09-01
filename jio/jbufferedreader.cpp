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
#include "jbufferedreader.h"
#include "jioexception.h"

#define LINE_SIZE	1024

namespace jio {

BufferedReader::BufferedReader(InputStream *stream_)
{
	jcommon::Object::SetClassName("jio::BufferedReader");

	if (stream_ == NULL) {
		throw IOException("InputStream null pointer exception");
	}

	_is_eof = false;
	_stream = stream_;
	
	_buffer = new char[4096];

	_buffer_size = 0;
	_buffer_index = 0;
}

BufferedReader::~BufferedReader()
{
	if ((void *)_buffer != NULL) {
		delete [] _buffer;
	}
}

int64_t BufferedReader::Available()
{
	return _stream->Available();
}

bool BufferedReader::IsEOF()
{
	return _is_eof;
}

int64_t BufferedReader::Read()
{
	int64_t r, d = _buffer_size - _buffer_index;
	char c;
	
	_is_eof = false;

	if (d == 0) {
		r = _stream->Read((char *)_buffer, 4096);
			
		if (r <= 0) {
			_is_eof = false;
			_buffer_size = 0;
			_buffer_index = 0;

			return -1;
		}

		_buffer_index = 0;
		_buffer_size = r;

		d = r;
	}
	
	c = _buffer[_buffer_index++];
	
	if (_buffer_index >= _buffer_size) {
		_buffer_index = _buffer_size = 0;
	}
	
	return (int64_t)c;

}

int64_t BufferedReader::Read(char *data, int64_t size)
{
	int64_t r, d, count = size;
	
	_is_eof = false;

	do {
		d = _buffer_size - _buffer_index;

		if (d == 0) {
			r = _stream->Read((char *)_buffer, 4096);

			if (r <= 0) {
				_is_eof = true;

				_buffer_size = 0;
				_buffer_index = 0;

				if (count == size) {
					return -1;
				} else {
					return size - count;
				}
			}

			_buffer_index = 0;
			_buffer_size = r;

			d = r;
		}

		r = count;

		if (r > d) {
			r = d;
		}

		memcpy((char *)(data + size - count), (char *)(_buffer + _buffer_index), (uint32_t)r);

		_buffer_index += r;
		count -= r;

		if (_buffer_index >= _buffer_size) {
			_buffer_index = _buffer_size = 0;
		}
	} while (count > 0);

	return size;
}

std::string BufferedReader::ReadLine(std::string delim)
{
	char *new_ptr,
			 *lineptr = new char[LINE_SIZE];
	int i,
			n = LINE_SIZE,
			x,
			tmp;

	const char *cdelim = delim.c_str();
	int csize = delim.size();

	_is_eof = false;

	for (i=0; ; ) {
		x = (int)Read();

		if (i >= n) {
			tmp = n+100;
			new_ptr = (char *)realloc(lineptr, tmp);

			if (new_ptr == NULL) {
				delete [] lineptr;
				return "";
			}

			lineptr = new_ptr;
			n = tmp;
		}

		if (x < 0) { 
			_is_eof = true;

			if (i == 0) {
				delete [] lineptr;

				return ""; 
			}

			// lineptr[i] = 0; 

			std::string str(lineptr, i);

			delete [] lineptr;

			return str; 
		}

		lineptr[i++] = x;

		if (i >= csize) {
			if (memcmp((lineptr + i - csize), cdelim, csize) == 0) {
				lineptr[i-csize] = 0;
				break;
			}
		}
	}

	// lineptr[i] = 0;

	std::string str(lineptr, i);

	delete [] lineptr;

	return str;
}

}
