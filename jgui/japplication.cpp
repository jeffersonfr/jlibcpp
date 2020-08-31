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
  static auto
    start_time = std::chrono::steady_clock::now();
  static int
    current_frame_number = 0;

  auto
    current_time = std::chrono::steady_clock::now();
  auto
    frame_time = start_time + current_frame_number++*std::chrono::microseconds{1000000/fps};

  std::this_thread::sleep_until(frame_time);

  if (current_time > frame_time) {
    if ((current_time - frame_time) > std::chrono::seconds(1)) {
      start_time = current_time;
      current_frame_number = 0;
    }

    return true;
  }

  return false;
}

}
