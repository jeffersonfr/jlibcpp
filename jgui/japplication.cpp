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
#include "jgui/japplication.h"

#include <iostream>

namespace jgui {

bool Application::FrameRate(size_t fps)
{
  static std::chrono::steady_clock::time_point 
    last_time = std::chrono::steady_clock::now();
  static size_t
    last_fps = fps,
    frames = 0;

  if (fps != last_fps) {
    last_time = std::chrono::steady_clock::now();
    last_fps = fps;
    frames = 0;
  }

  static std::chrono::steady_clock::time_point 
    current = std::chrono::steady_clock::now();
  static std::chrono::steady_clock::time_point 
    calculate = last_time + std::chrono::microseconds(++frames*1000000LL/fps);

  if (calculate < current) {
    return true;
  }

  std::this_thread::sleep_for(calculate - current);

  return false;
}

}
