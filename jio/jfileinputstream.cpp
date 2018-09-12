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
#include "jio/jfileinputstream.h"
#include "jexception/jioexception.h"
#include "jexception/jnullpointerexception.h"

namespace jio {

FileInputStream::FileInputStream(std::string filename_):
	jio::InputStream()
{
	jcommon::Object::SetClassName("jio::FileInputStream");

	_file = jio::File::OpenFile(filename_);

	if (_file == nullptr) {
		throw jexception::IOException("Unable to open file");
	}
	
	_flag = 0;
}

FileInputStream::FileInputStream(File *file_):
	jio::InputStream()
{
	jcommon::Object::SetClassName("jio::FileInputStream");

	_file = file_;

	if (_file == nullptr) {
		throw jexception::NullPointerException("File must be a valid pointer");
	}

	_flag = 1;
}

FileInputStream::~FileInputStream()
{
	if (_flag == 0 && (void *)_file != nullptr) {
		// _file->Close();
		delete _file;
	}
}

bool FileInputStream::IsEmpty()
{
	return (_file->Tell() >= _file->GetSize());
}

int64_t FileInputStream::Available()
{
	return (_file->GetSize() - _file->Tell());
}

int64_t FileInputStream::GetSize()
{
	return _file->GetSize();
}

int64_t FileInputStream::GetPosition()
{
	return _file->Tell();
}

int64_t FileInputStream::Read()
{
	if (IsEmpty() == true) {
		return -1LL;
	}

	char c = 0;
	
	if (_file->Read(&c, 1LL) <= 0LL) {
		return -1LL;
	}
	
	return (int64_t)c;
}

int64_t FileInputStream::Read(char *data, int64_t size)
{
	if ((void *)data == nullptr) {
		return -1LL;
	}

	if (IsEmpty() == true) {
		return -1LL;
	}

	int r;
   
	r = _file->Read((char *)data, size);

	if (r <= 0) {
		return -1LL;
	}

	return r;
}

void FileInputStream::Skip(int64_t skip)
{
	if (skip <= 0LL) {
		return;
	}

	int64_t r = skip;

	if (skip > Available()) {
		r = Available();
	}

	_file->Seek(r);
}

void FileInputStream::Reset()
{
	_file->Reset();
}

void FileInputStream::Close()
{
	_file->Close();
}

int64_t FileInputStream::GetReadedBytes()
{
	return _file->Tell();
}

}
