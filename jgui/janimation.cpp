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
#include "jgui/janimation.h"

#include <algorithm>

namespace jgui {

Animation::Animation(std::chrono::milliseconds start, std::chrono::milliseconds interval, bool loop):
  Component()
{
  SetClassName("jgui::Animation");

  _timestamp = std::chrono::steady_clock::now();
  _start = start;
  _interval = interval;
  _loop = loop;
  _running = false;
}

Animation::~Animation()
{
  Stop();
}

void Animation::Paint(Graphics *g)
{
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _timestamp);

  if (_start >= std::chrono::milliseconds(0) and elapsed > _start) {
    _start = std::chrono::milliseconds(-1);
    _timestamp = now;

    Update(_start);
  } else {
    if (elapsed > _interval) {
      _timestamp = now;

      Update(_interval);
    }
  }

  Render(g);
  Repaint();
}

void Animation::SetLoop(bool param)
{
  _loop = param;
}

bool Animation::IsLoop()
{
  return _loop;
}

void Animation::SetInterval(std::chrono::milliseconds interval)
{
  _interval = interval;
}

std::chrono::milliseconds Animation::GetInterval()
{
  return _interval;
}

void Animation::Start()
{
  _timestamp = std::chrono::steady_clock::now();

  _running = true;
}

void Animation::Stop()
{
  _running = false;
}

void Animation::Update(std::chrono::milliseconds tick)
{
}

void Animation::Render(Graphics *g)
{
}

}
