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
#include "jevent/jwindowlistener.h"

namespace jevent {

WindowListener::WindowListener():
  jevent::Listener()
{
  jcommon::Object::SetClassName("jevent::WindowListener");
}

WindowListener::~WindowListener()
{
}

void WindowListener::WindowOpened(WindowEvent *event)
{
}

void WindowListener::WindowClosing(WindowEvent *event)
{
}

void WindowListener::WindowClosed(WindowEvent *event)
{
}

void WindowListener::WindowResized(WindowEvent *event)
{
}

void WindowListener::WindowMoved(WindowEvent *event)
{
}

void WindowListener::WindowPainted(WindowEvent *event)
{
}

void WindowListener::WindowEntered(WindowEvent *event)
{
}

void WindowListener::WindowLeaved(WindowEvent *event)
{
}

}
