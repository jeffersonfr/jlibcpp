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
#include "jio/joutputstream.h"

namespace jio {

OutputStream::OutputStream()
{
    jcommon::Object::SetClassName("jio::OutputStream");

  _blocked = true;
}

OutputStream::~OutputStream()
{
}

void OutputStream::SetBlocking(bool block_)
{
  _blocked = block_;
}

bool OutputStream::IsBlocking()
{
  return _blocked;
}

bool OutputStream::IsClosed()
{
  return _is_closed;
}

int64_t OutputStream::Write(std::string str)
{
  return Write((char *)str.c_str(), str.size());
}

bool OutputStream::IsEmpty()
{
  return false;
}

int64_t OutputStream::Available()
{
  return 0LL;
}

int64_t OutputStream::GetSize()
{
  return 0LL;
}

int64_t OutputStream::Write(int64_t b)
{
  return 0LL;
}

int64_t OutputStream::Write(const char *data, int64_t size)
{
  return 0LL;
}

int64_t OutputStream::Flush()
{
  return 0LL;
}

void OutputStream::Seek(int64_t index)
{
}

void OutputStream::Close()
{
}

int64_t OutputStream::GetSentBytes()
{
  return 0LL;
}

}
