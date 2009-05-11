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
#include "jbitinputstream.h"
#include "jfileinputstream.h"
#include "jioexception.h"
#include "joutofboundsexception.h"

#include <iostream>
#include <string>
#include <sstream>

#include <time.h>

namespace jio {

#define MASKS_LENGTH	8

int masks[8] = {
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

BitInputStream::BitInputStream(std::string filename):
	InputStream()
{
	jcommon::Object::SetClassName("jio::BitInputStream");
	
	haveByte = false;
	show = false;
	currentMask = 0;
	currentByte = 0;
     
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

long long BitInputStream::Available()
{
	return stream->Available();
}

long long BitInputStream::GetSize()
{
	return file->GetSize();
}

long long BitInputStream::GetPosition()
{
	return 0LL;
}

int BitInputStream::Read()
{
	int next = stream->Read();
	
	if (next < 0) {
		return -1;
	}
	
	return next;
}

long long BitInputStream::Read(char *buffer, long long size)
{
	long long i;
	int r;
	
	for (i=0; i<size; i++) {
		if ((r = Read()) > 0) {
			buffer[i] = r;
		}
	}
	
	return i;
}

void BitInputStream::Skip(long long skip)
{
	stream->Skip(skip);
}

void BitInputStream::Reset()
{
	// TODO:: seek file to begin
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

long long BitInputStream::GetReceiveBytes()
{
	return 0LL;
}

}

