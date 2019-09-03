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
#include "jmpeg/jdatastream.h"
#include "jmpeg/jmpeglib.h"
#include "jexception/joverflowexception.h"
#include "jexception/joutofboundsexception.h"
#include "jexception/jinvalidargumentexception.h"

namespace jmpeg {

DataStream::DataStream(std::string &data, size_t lo, size_t hi):
	DataStream(data)
{
	_data_index_lo = lo;
	_data_index_hi = hi;
}

DataStream::DataStream(std::string &data):
	_data(data)
{
	_data_index = 0;
	_data_index_lo = 0;
	_data_index_hi = data.size();
}

DataStream::DataStream(std::string &&data):
	_data(data)
{
	_data_index = 0;
	_data_index_lo = 0;
	_data_index_hi = data.size();
}

DataStream::DataStream(const char *data, int length):
  DataStream(std::string(data, length))
{
  if (length < 0) {
    throw jexception::OutOfBoundsException("Data length must be greater than zero");
  }
}

DataStream::~DataStream()
{
}

DataStream DataStream::Slice(size_t lo, size_t hi)
{
	lo = _data_index_lo + lo;
	hi = hi;

  if (lo > hi) {
    throw jexception::InvalidArgumentException("Higher index must be greater than lower index");
  }

	if (lo > (_data_index_hi - 1) or hi > _data_index_hi) {
    throw jexception::OutOfBoundsException("Indexes are out of bounds");
  }

	return DataStream(_data, lo, hi);
}

void DataStream::SetBits(uint64_t bits, size_t n)
{
  if ((_data_index + n) > (_data_index_hi << 3)) {
    throw jexception::OverflowException("Set overflow");
  }

  DataStream stream((const char *)&bits, 8);

  stream.Skip(64 - n);

  uint8_t 
    *ptr = (uint8_t *)_data.data();

  while (GetAvailableBits() > 0) {
    int 
      r = _data_index%8,
        d = 8 - r;

    if ((uint64_t)d > bits) {
      d = bits;
    }

    TS_SM8(ptr + (_data_index >> 3), r, d, (uint8_t)stream.GetBits(d));

    _data_index = _data_index + d;
  }
}

void DataStream::SetBitsAsString(std::string bits)
{
  for (size_t i=0; i<bits.size(); i++) {
    (bits[i] == '0')?SetBits(1, 1):SetBits(0, 1);
  }
}

void DataStream::SetBytes(std::string bytes) 
{
  for (size_t i=0; i<bytes.size(); i++) {
    SetBits(bytes[i], 8);
  }
}

uint64_t DataStream::GetBits(size_t n)
{
  if ((_data_index + n) > (_data_index_hi << 3)) {
    throw jexception::OverflowException("Get overflow");
  }

  if (n > 64L) {
    throw jexception::OutOfBoundsException("Maximum limit was exceded");
  }

  if (n == 0) {
    return 0LL;
  }

  uint8_t 
    *ptr = (uint8_t *)_data.c_str();
  uint64_t 
    bits = 0LL;

  /*
  int
    offset = _data_index%8;

  if (offset != 0) {
    int d = n;

    if ((offset + n) > 8) {
      d = 8 - offset;
    }

    bits = TS_GM8(ptr + (_data_index >> 3), offset, d);
    n = n - d;
    _data_index = _data_index + n;
  }

  if (n > 0) {
    bits = (bits << n) | TS_GM64(ptr + (_data_index >> 3), 0, n);
    _data_index = _data_index + n;
  }
  */

  size_t 
    start = 0,
    end = 0;

  do {
    start = _data_index >> 3;
    end = (_data_index + n - 1) >> 3;

    if (start == end) {
      bits = (bits << n) | TS_GM8(ptr + start, _data_index%8, n);

      _data_index = _data_index + n;

      n = 0;
    } else {
      size_t 
        d = 8 - (_data_index%8);

      bits = (bits << d) | TS_GM8(ptr + start, _data_index%8, d);

      _data_index = _data_index + d;

      n = n - d;
    }
  } while (n > 0);

  return bits;
}

std::string DataStream::GetBitsAsString(size_t n)
{
  std::string bits;

  if (n == 0) {
    return bits;
  }

  bits.reserve(n);

  for (size_t i=0; i<n; i++) {
    bits = bits + ((GetBits(1) == 0)?'0':'1');
  }

  return bits;
}

std::string DataStream::GetBytes(size_t n)
{
  std::string bytes;

  if (n == 0) {
    return bytes;
  }

  bytes.reserve(n);

  for (size_t i=0; i<n; i++) {
    uint8_t byte = GetBits(8);

    bytes.append((const char *)&byte, 1);
  }

  return bytes;
}

void DataStream::Skip(size_t n)
{
  if ((_data_index + n) > (_data_index_hi << 3)) {
    throw jexception::OverflowException("Skip overflow");
  }

  _data_index = _data_index + n;
}

void DataStream::Reset()
{
  _data_index = _data_index_lo;
}

size_t DataStream::GetAvailableBits()
{
  return (_data_index_hi << 8) - _data_index;
}

size_t DataStream::GetAvailableBytes()
{
  return GetAvailableBits() >> 8;
}

uint8_t DataStream::GetRawByte(size_t index)
{
  if ((_data_index_lo + index) >= _data_index_hi) {
    throw jexception::OverflowException("Skip overflow");
  }

  return *((uint8_t *)_data.c_str() + _data_index_lo + index);
}

std::string DataStream::GetRawBytes(size_t index, size_t n)
{
  std::string bytes;

  if (n == 0) {
    return bytes;
  }

  bytes.reserve(n);

  for (size_t i=0; i<n; i++) {
    uint8_t byte = GetRawByte(index + i);

    bytes.append((const char *)&byte, 1);
  }

  return bytes;
}

}

