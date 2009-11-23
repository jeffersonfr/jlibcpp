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
#include "jfileinputstream.h"
#include "jioexception.h"
#include "jnullpointerexception.h"

#include <iostream>
#include <string>
#include <sstream>

#include <time.h>

namespace jio {

FileInputStream::FileInputStream(std::string filename_):
	jio::InputStream()
{
    jcommon::Object::SetClassName("jio::FileInputStream");

	try {
		_flag = 0;
		_file = new File(filename_);

		_buffer = new uint8_t[4096];

		_buffer_size = 0;
		_buffer_index = 0;
		_current = 0;
	} catch (...) {
		_file = NULL;

		throw IOException("Open file failed");
	}
}

FileInputStream::FileInputStream(File *file_):
	jio::InputStream()
{
    jcommon::Object::SetClassName("jio::FileInputStream");

	_flag = 1;
	_file = file_;

	_buffer = new uint8_t[65535];

	_buffer_size = 0;
	_buffer_index = 0;
	_current = 0;

	if (_file == NULL) {
		throw jcommon::NullPointerException("File pointer is null");
	}
}

FileInputStream::~FileInputStream()
{
	if (_flag == 0 && (void *)_file != NULL) {
		// _file->Close();
		delete _file;
	}

	if ((void *)_buffer != NULL) {
		delete [] _buffer;
	}
}

bool FileInputStream::IsEmpty()
{
	return (_current >= GetSize());
}

int64_t FileInputStream::Available()
{
	return GetSize() - GetPosition();
}

int64_t FileInputStream::GetSize()
{
	return _file->GetSize();
}

int64_t FileInputStream::GetPosition()
{
	return _current;
}

int FileInputStream::Read()
{
	if (IsEmpty() == true) {
		return -1;
	}

	/* correct:: slow
	char c = 0,
		 r;
	
	r = _file->Read(&c, 1);
	
	if (r < 0) {
		return -1;
	}
	
	_current++;
	
	return (int)c;
	*/

	int r,
		d = _buffer_size - _buffer_index;
	char c;
	
	if (d == 0) {
		r = _file->Read((char *)_buffer, 4096);
			
		if (r <= 0) {
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
	
	_current++;

	return (int)c;
}

int64_t FileInputStream::Read(char *data, int64_t size)
{
	if ((void *)data == NULL) {
		return -1;
	}

	if (IsEmpty() == true) {
		return -1;
	}

	/*
	int r;
   
	r = _file->Read((char *)data, size);

	if (r <= 0) {
		return -1;
	}

	_current += r;

	return r;
	*/

	int r,
		d,
		count = size;
	
	do {
		d = _buffer_size - _buffer_index;

		if (d == 0) {
			r = _file->Read((char *)_buffer, 4096);

			if (r <= 0) {
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

		memcpy((char *)(data + size - count), (char *)(_buffer + _buffer_index), r);

		_buffer_index += r;
		_current += r;
		count -= r;

		if (_buffer_index >= _buffer_size) {
			_buffer_index = _buffer_size = 0;
		}
	} while (count > 0);

	return size;
}

void FileInputStream::Skip(int64_t skip)
{
	if (skip <= 0) {
		return;
	}

	int r = skip;

	if (skip > Available()) {
		r = Available();
	}

	char *tmp = new char[r];

	r = _file->Read((char *)tmp, r);

	delete [] tmp;
}

void FileInputStream::Reset()
{
	_file->Reset();

	_current = 0;
}

void FileInputStream::Close()
{
	_file->Close();
}

int64_t FileInputStream::GetReceiveBytes()
{
	return _current;
}

}
