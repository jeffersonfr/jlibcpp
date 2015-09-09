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
#include "jbitinputstream.h"
#include "jfileinputstream.h"
#include "jioexception.h"
#include "joutofboundsexception.h"

namespace jio {

#define MASKS_LENGTH	8

static int masks[8] = {
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

BitInputStream::BitInputStream(std::string filename):
	InputStream()
{
	jcommon::Object::SetClassName("jio::BitInputStream");

	haveByte = false;
	show = false;
	currentMask = 0LL;
	currentByte = 0LL;

	try {
		file = new File(filename);
		stream = new FileInputStream(file);
	} catch (...) {
		if (file != NULL) {
			delete file;
		}

		file = NULL;
		stream = NULL;

		throw IOException("Cannot open file in BitInputStream");
	}
}

BitInputStream::BitInputStream(InputStream *is):
	InputStream()
{
	jcommon::Object::SetClassName("jio::BitInputStream");

	if ((void *)is == NULL) {
		throw IOException("Null pointer exception");
	}

	stream = is;
	haveByte = false;
	show = false;
	currentMask = 0;
	currentByte = 0;
	file = NULL;
}

BitInputStream::~BitInputStream()
{
	if (stream != NULL) {
		delete stream;
	}

	if (file != NULL) {
		delete file;
	}
}

bool BitInputStream::IsEmpty()
{
	return Available() == 0;
}

int64_t BitInputStream::Available()
{
	return stream->Available();
}

int64_t BitInputStream::GetSize()
{
	return file->GetSize();
}

int64_t BitInputStream::GetPosition()
{
	return 0LL;
}

int64_t BitInputStream::Read()
{
	int64_t next = stream->Read();

	if (next < 0LL) {
		return -1LL;
	}

	return next;
}

int64_t BitInputStream::Read(char *buffer, int64_t size)
{
	int64_t i,
		r;

	for (i=0; i<size; i++) {
		if ((r = Read()) > 0LL) {
			buffer[i] = (char)r;
		}
	}

	return i;
}

void BitInputStream::Skip(int64_t skip)
{
	stream->Skip(skip);
}

void BitInputStream::Reset()
{
	stream->Reset();
}

int BitInputStream::ReadBit()
{
	if (haveByte == false) {
		currentByte = Read();
		haveByte = true;
	}

	int value = (currentByte & masks[currentMask]) / masks[currentMask];

	currentMask++;
	if (currentMask == MASKS_LENGTH) {
		haveByte = false;
		currentMask = 0;
	}

	return value;
}

int BitInputStream::ReadBits(int num) 
{
	if ((num < 0) || (num > 32)) {
		throw new jcommon::OutOfBoundsException("Number of bits is out of range");
	}

	int bits = 0;
	for (int i=0; i<num; i++) {
		bits = (bits << 1) | ReadBit();
	}

	return bits;
}

void BitInputStream::Close()
{
	stream->Close();
}

int64_t BitInputStream::GetReadedBytes()
{
	return 0LL;
}

}

