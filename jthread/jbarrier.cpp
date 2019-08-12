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
#include "jthread/jbarrier.h"

namespace jthread {

Barrier::Barrier(size_t counter):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jthread::Barrier");

  _phase = 0;
  _start_counter = counter;
  _counter = _counter;
}

Barrier::~Barrier()
{
}

void Barrier::UnlockBarrier(bool drop)
{
  std::unique_lock<std::mutex> lock(_mutex);

  size_t arrival_phase = _phase;

  _counter = _counter - 1;

  if (_counter == 0) {
    if (drop) {
      _start_counter = _start_counter - 1;
    }

    _counter = _start_counter;
    _phase = _phase + 1;

    _condition.notify_all();
  }

  while (_phase <= arrival_phase) {
    _condition.wait(lock);
  }
}

size_t Barrier::Arrive(size_t update)
{
  std::lock_guard<std::mutex> lock(_mutex);

  size_t token = _phase;

  _counter = _counter - update;

  if (_counter == 0) {
    UnlockBarrier(false);
  }

  return token;
}

void Barrier::Wait(size_t phase) const
{
  std::unique_lock<std::mutex> lock(_mutex);

  while (_phase <= phase) {
    _condition.wait(lock);
  }
}

void Barrier::ArriveAndWait()
{
  UnlockBarrier(false);
}

void Barrier::ArriveAndDrop()
{
  UnlockBarrier(true);
}

}
