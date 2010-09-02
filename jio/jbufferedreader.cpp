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
}

BufferedReader::~BufferedReader()
{
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
	int64_t r;

	r = _stream->Read();

	if (r < 0LL) {
		_is_eof = true;
	} else {
		_is_eof = false;
	}

	return r;
}

int64_t BufferedReader::Read(char *data, int64_t size)
{
	int64_t r;

	r = _stream->Read(data, size);

	if (r < 0LL) {
		_is_eof = true;
	} else {
		_is_eof = false;
	}

	return r;
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
		x = (int)_stream->Read();

		if (i >= n) {
			tmp = n+100;
			new_ptr = (char *)realloc(lineptr, tmp);

			if (new_ptr == NULL) {
				delete lineptr;
				return "";
			}

			lineptr = new_ptr;
			n = tmp;
		}

		if (x < 0) { 
			_is_eof = true;

			if (i == 0) {
				delete lineptr;
				return ""; 
			}

			lineptr[i] = 0; 

			return lineptr; 
		}

		lineptr[i++] = x;

		if (i >= csize) {
			if (memcmp((lineptr + i - csize), cdelim, csize) == 0) {
				lineptr[i-csize] = 0;
				break;
			}
		}
	}

	lineptr[i] = 0;

	return lineptr;
}

}
