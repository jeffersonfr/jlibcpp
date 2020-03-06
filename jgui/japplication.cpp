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

bool Application::FrameRate(int fps)
{
  static std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
  static int last_fps = fps;

  if (fps != last_fps) {
    last_time = std::chrono::steady_clock::now();
    last_fps = fps;
  }

  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
  std::chrono::milliseconds step = std::chrono::milliseconds(1000/fps);

  last_time = now;

  if (diff > step) {
    return true;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(step - diff));

  return false;

  /*
  static std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  static int latest_fps = -1;
  static int index = 0;

  if (fps != latest_fps) {
    begin = std::chrono::steady_clock::now();
    latest_fps = fps;
    index = 0;
  }

  std::chrono::time_point<std::chrono::steady_clock> timestamp = begin + std::chrono::milliseconds(++index*1000/fps);
  std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
  std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - current);

  if (diff.count() < 0) {
    std::chrono::milliseconds step(1000/fps);

    step = 2*step;

    if (step < -diff) {
      return true; // skip frame
    }

    return false;
  }

  std::this_thread::sleep_for(diff);

  return false;
  */
}

}
