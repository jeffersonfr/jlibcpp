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
#include "jshared/jpipe.h"

#include <unistd.h>

namespace jshared {

Pipe::Pipe(int size_, int perms_):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jshared::Pipe");
    
  _is_closed = false;

  if (pipe(_pipe) < 0) {
    _is_closed = true;
  }
}

Pipe::~Pipe()
{
  Close();
}

int Pipe::Receive(char *data_, int length_)
{
  if (_is_closed == true) {
    return 0;
  }

  return read(_pipe[0], data_, length_);
}

int Pipe::Send(const char *data_, int length_)
{
  if (_is_closed == true) {
    return 0;
  }

  return write(_pipe[1], data_, length_);
}

bool Pipe::IsClosed()
{
  return _is_closed;
}

void Pipe::Close()
{
  if (_is_closed == false) {
    _is_closed = true;

    close(_pipe[0]);
    close(_pipe[1]);
  }
}

}
