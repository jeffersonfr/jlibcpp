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
#include "jcommonlib.h"

namespace jcommon {
	
const int mask[][8] = {
	{0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01},
	{0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x00},
	{0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x07, 0x00, 0x00},
	{0xf0, 0x78, 0x3c, 0x1e, 0x0f, 0x00, 0x00, 0x00},
	{0xf8, 0x7c, 0x3e, 0x1f, 0x00, 0x00, 0x00, 0x00},
	{0xfc, 0x7e, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0xfe, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

BitStream::BitStream(char *stream, int size):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::BitStream");
	
	if (stream == NULL || size == 0) {
		_stream = NULL;
		
		throw RuntimeException("Null pointer reference !");
	}

	_stream = stream;
	_size = size;
 
	Reset();
}

BitStream::~BitStream()
{
	if (_stream != NULL) {
		delete [] _stream;
	}
}

void BitStream::Reset() {
	_index = 0;
	_offset = 0;
	_consumed = 0;
}
	
uint64_t BitStream::GetBits(int n) 
{
	// WARNNING: veridy bug when n > 24
	
	uint64_t result = 0LL;
	int bitsToConsume = n;
	int bitsToConsumeNow;
	
	while (bitsToConsume != 0) {
		bitsToConsumeNow = 8 - _offset;
		
		if (bitsToConsumeNow > bitsToConsume) {
			bitsToConsumeNow = bitsToConsume;
		}
		
		result |= mask[bitsToConsumeNow - 1][_offset] & _stream[_index];
		bitsToConsume -= bitsToConsumeNow;
		_offset += bitsToConsumeNow;
		
		if (_offset == 8) {
			result <<= 8;
			_index += 1;
			_offset = 0;
			_consumed++;
		}
	}

	result >>= 8 - _offset;				

	return result;
}

uint8_t* BitStream::GetBitsArray(int size) 
{
	int i,
		len8 = size / 8,
		mod8 = size % 8;
	
	try {
		uint8_t*array = new uint8_t[len8];
	
		for (i=0; i<len8; i++) {
			array[i] = (uint8_t)GetBits(8);
		}

		array[i] = (uint8_t)(GetBits(mod8) << (8 - mod8));
	
		return array;
	} catch (std::bad_alloc &) {
		return NULL;
	}

	return NULL;
}

int BitStream::Skip(int n) {
	_index += n / 8;
	_offset += n % 8;

	if (_offset >= 8) {
		_offset %= 8;
		_index++;
	}

	return _index * 8 + _offset;
}

int BitStream::Back(int n) {
	_offset -= n % 8;

	if (_offset < 0) {
		_offset += 8;
		_index--;
	}

	return _index * 8 + _offset;
}

int BitStream::GetAvailable()
{
	return (_size - _index) * 8 + (8 - _offset);
}

int BitStream::GetPosition()
{
	return _index * 8 + _offset;
}

int BitStream::SetPosition(int p)
{
	_index = p / 8;
	_offset = p % 8;

	return _index * 8 + _offset;
}

bool BitStream::TestBit(int index)
{
   	if (index > _size) {
	   	throw std::out_of_range("Index is out of range !");
	}  
	
	return (_stream[(unsigned int)(index/8)] & (1 << (unsigned int)(index%8))) != 0;
}

}
