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
#include "jthread/jlatch.h"

namespace jthread {

Latch::Latch():
  jcommon::Object()
{
  jcommon::Object::SetClassName("jthread::Latch");

  _counter.store(0);
}

Latch::~Latch()
{
}

void Latch::Add(size_t n)
{
  _counter.fetch_add(n);
}

void Latch::Arrive()
{
  _counter.fetch_sub(1);
  _condition.notify_all();
}

void Latch::Wait()
{
  std::unique_lock<std::mutex> lock(_mutex);

  _condition.wait(lock,
    [&]() { 
      return _counter <= 0; 
    });
}

void Latch::ArriveAndWait()
{
  std::unique_lock<std::mutex> lock(_mutex);

  _counter.fetch_sub(1);
  _condition.notify_all();

  _condition.wait(lock,
    [&]() { 
      return _counter <= 0; 
    });
}

}
