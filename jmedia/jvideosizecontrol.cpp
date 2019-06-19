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
#include "jmedia/jvideosizecontrol.h"

namespace jmedia {

VideoSizeControl::VideoSizeControl():
  Control("video.size")
{
  jcommon::Object::SetClassName("jmedia::VideoSizeControl");
}
    
VideoSizeControl::~VideoSizeControl()
{
}

void VideoSizeControl::SetSize(jgui::jsize_t<int> t)
{
  SetSize(t);
}

void VideoSizeControl::SetSource(jgui::jrect_t<int> t)
{
  SetSource(t);
}

void VideoSizeControl::SetDestination(jgui::jrect_t<int> t)
{
  SetDestination(t);
}

void VideoSizeControl::SetSize(int w, int h)
{
}

void VideoSizeControl::SetSource(int x, int y, int w, int h)
{
}

void VideoSizeControl::SetDestination(int x, int y, int w, int h)
{
}

jgui::jsize_t<int> VideoSizeControl::GetSize()
{
  return {0, 0};
}

jgui::jrect_t<int> VideoSizeControl::GetSource()
{
  return {0, 0, 0, 0};
}

jgui::jrect_t<int> VideoSizeControl::GetDestination()
{
  return {0, 0, 0, 0};
}

}
