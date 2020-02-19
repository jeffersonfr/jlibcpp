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

Animation::Animation(jgui::jrect_t<int> bounds):
   Component(bounds)
{
  SetClassName("jgui::Animation");

  _index = 0;
  _running = false;

  SetInterval(500);
}

Animation::~Animation()
{
  Stop();
}

void Animation::Start()
{
   std::lock_guard<std::mutex> guard(_animation_mutex);

  if (_running == true) {
    return;
  }

  _running = true;

  _thread = std::thread(&Animation::Run, this);
}

void Animation::Stop()
{
   std::lock_guard<std::mutex> guard(_animation_mutex);

  if (_running == false) {
    return;
  }

  _running = false;

  try {
    _thread.join();
  } catch (...) {
  }
}

void Animation::SetInterval(int i)
{
  _interval = i;
}

void Animation::AddImage(jgui::Image *image)
{
   std::lock_guard<std::mutex> guard(_animation_mutex);

  _images.push_back(image);
}

void Animation::RemoveImage(jgui::Image *image)
{
   std::lock_guard<std::mutex> guard(_animation_mutex);

  _images.erase(std::remove(_images.begin(), _images.end(), image), _images.end());
}

void Animation::RemoveAll()
{
   std::lock_guard<std::mutex> guard(_animation_mutex);

  _images.clear();
}

const std::vector<jgui::Image *> & Animation::GetImages()
{
  return _images;
}

void Animation::Paint(Graphics *g)
{
  // JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jgui::jsize_t
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  int 
    w = size.width - GetHorizontalPadding(),
    h = size.height - GetVerticalPadding();

  if (_images.size() != 0) {
    Image *image = _images[_index];

    g->DrawImage(image, {GetPadding().left, GetPadding().top, w, h});
  }
}

void Animation::Run()
{
  while (_running == true) {
    Repaint();

    if (_running == false) {
      return;
    }

    _index++;

    if (_index >= (int)_images.size()) {
      _index = 0;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(_interval));
  }
}

}
