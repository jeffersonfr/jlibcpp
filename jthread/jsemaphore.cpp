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
#include "jthread/jsemaphore.h"

namespace jthread {

Semaphore::Semaphore(size_t counter):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jthread::Semaphore");

  _counter = counter;
}

Semaphore::~Semaphore()
{
}

bool Semaphore::TryWait()
{
  std::lock_guard<std::mutex> lock(_mutex);

  if (_counter > 0) {
    _counter = _counter - 1;

    return true;
  }

  return false;
}

void Semaphore::Wait()
{
  std::unique_lock<std::mutex> lock(_mutex);

  _condition.wait(lock, 
      [&] { 
        return _counter > 0; 
      });

  _counter = _counter - 1;
}

bool Semaphore::WaitFor(const std::chrono::milliseconds &ms)
{
  std::unique_lock<std::mutex> lock(_mutex);

  bool finished = _condition.wait_for(lock, ms, 
      [&] { 
        return _counter > 0; 
      });

  if (finished == true) {
    _counter = _counter - 1;
  }

  return finished;
}

bool Semaphore::WaitUntil(const std::chrono::steady_clock::time_point &tp)
{
  std::unique_lock<std::mutex> lock(_mutex);

  auto finished = _condition.wait_until(lock, tp, 
      [&] { 
        return _counter > 0; 
      });

  if (finished == true) {
    _counter = _counter - 1;
  }

  return finished;
}

void Semaphore::Notify()
{
  std::lock_guard<std::mutex> lock(_mutex);

  _counter = _counter + 1;

  _condition.notify_one();
}

size_t Semaphore::GetCounter()
{
  // std::lock_guard<std::mutex> lock(_mutex);

  return _counter;
}

}
