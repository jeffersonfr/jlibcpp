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

namespace jmpeg {

DataStream::DataStream(std::string data)
{
  _data = data;
}

DataStream::DataStream(const char *data, size_t length):
  DataStream(std::string(data, length))
{
  _data_index = 0;
}

DataStream::~DataStream()
{
}

void DataStream::SetBits(uint64_t bits, size_t n)
{
  if ((_data_index + n) > (_data.size() << 3)) {
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
  if ((_data_index + n) > (_data.size() << 3)) {
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
  if ((_data_index + n) > (_data.size() << 3)) {
    throw jexception::OverflowException("Skip overflow");
  }

  _data_index = _data_index + n;
}

void DataStream::Reset()
{
  _data_index = 0;
}

size_t DataStream::GetAvailableBits()
{
  return (_data.size() << 8) - _data_index;
}

size_t DataStream::GetAvailableBytes()
{
  return GetAvailableBits() >> 8;
}

}

