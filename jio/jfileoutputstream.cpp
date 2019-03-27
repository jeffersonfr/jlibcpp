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
#include "jio/jfileoutputstream.h"
#include "jexception/jioexception.h"
#include "jexception/jnullpointerexception.h"

#include <string.h>

namespace jio {

FileOutputStream::FileOutputStream(std::string filename_):
	jio::OutputStream()
{
	jcommon::Object::SetClassName("jio::FileOutputStream");

	_buffer_length = 4096;
	_current_index = 0;
	_sent_bytes = 0;
	
	try {
		_buffer = new char[(int)_buffer_length];
	} catch (std::bad_alloc &) {
		throw jexception::IOException("Out of memory");
	}

	try {
		_file = jio::File::OpenFile(filename_, (jio::jfile_flags_t)(JFF_READ_WRITE | JFF_LARGEFILE));

		if (_file == nullptr) {
			_file = jio::File::CreateFile(filename_, (jio::jfile_flags_t)(JFF_READ_WRITE | JFF_LARGEFILE));
		}
	} catch (...) {
		_file = nullptr;

		throw jexception::IOException("Open file failed");
	}
  
  _flag = 0;
}

FileOutputStream::FileOutputStream(File *file_):
	jio::OutputStream()
{
    jcommon::Object::SetClassName("jio::FileOutputStream");

	_file = file_;

	if (_file == nullptr) {
		throw jexception::NullPointerException("File pointer is null");
	}

	_buffer_length = 4096;
	_current_index = 0;
	_sent_bytes = 0;
	
	try {
		_buffer = new char[(int)_buffer_length];
	} catch (std::bad_alloc &) {
		throw jexception::IOException("Out of memory");
	}

  _flag = 1;
}

FileOutputStream::~FileOutputStream()
{
	if (_flag == 0 && (void *)_file != nullptr) {
    _file->Close();

		delete _file;
    _file = nullptr;
	}

	if ((void *)_buffer != nullptr) {
		delete [] _buffer;
	}
}

bool FileOutputStream::IsEmpty()
{
	return Available() == 0;
}

int64_t FileOutputStream::Available()
{
	return 0LL;
}

int64_t FileOutputStream::GetSize()
{
	return _file->GetSize();
}

int64_t FileOutputStream::Write(int64_t b)
{
	_buffer[_current_index++] = (uint8_t)b;

	if (_current_index == _buffer_length) {
		return Flush();
	}

	return 0LL;
}

int64_t FileOutputStream::Write(const char *data_, int64_t data_length_)
{
	int64_t size,
		 l = data_length_;
	
	while (l > 0LL) {
		size = (_buffer_length - _current_index);

		if (l < size) {
			memcpy((_buffer + _current_index), (data_ + (int)(data_length_ - l)), (size_t)l);

			_current_index += l;
			l = 0;

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

int64_t FileOutputStream::Flush()
{
	if (_current_index == 0) {
		return 0LL;
	}

	int64_t n;

	n = _file->Write(_buffer, (int)_current_index);

	_current_index = 0LL;

	if (n <= 0LL) {
		return -1LL;
	}

	_sent_bytes += n;

	return (int64_t)n;
}

void FileOutputStream::Seek(int64_t index)
{
	_file->Seek(index);
}

void FileOutputStream::Close()
{
	_file->Close();
}

int64_t FileOutputStream::GetSentBytes()
{
	return _sent_bytes;
}

}
