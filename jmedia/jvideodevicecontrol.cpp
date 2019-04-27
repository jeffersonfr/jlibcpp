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
#include "jmedia/jvideodevicecontrol.h"

namespace jmedia {

VideoDeviceControl::VideoDeviceControl():
  Control("video.device")
{
  jcommon::Object::SetClassName("jmedia::VideoDeviceControl");
}
    
VideoDeviceControl::~VideoDeviceControl()
{
}

const std::vector<jvideo_control_t> & VideoDeviceControl::GetControls()
{
  return _controls;
}

bool VideoDeviceControl::HasControl(jvideo_control_t id)
{
  return false;
}

int VideoDeviceControl::GetValue(jvideo_control_t id)
{
  return -1;
}

bool VideoDeviceControl::SetValue(jvideo_control_t id, int value)
{
  return false;
}

void VideoDeviceControl::Reset(jvideo_control_t id)
{
}

}
