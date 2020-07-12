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
#include "jgui/jwindowadapter.h"

namespace jgui {

WindowAdapter::WindowAdapter():
  Container()
{
  jcommon::Object::SetClassName("jgui::WindowAdapter");
}

WindowAdapter::~WindowAdapter()
{
}

void WindowAdapter::SetResizable(bool resizable)
{
}

bool WindowAdapter::IsResizable()
{
  return false;
}

void WindowAdapter::ToggleFullScreen()
{
}

jcursor_style_t WindowAdapter::GetCursor()
{
  return JCS_DEFAULT;
}

void WindowAdapter::SetBounds(int x, int y, int width, int height)
{
}

jgui::jrect_t<int> WindowAdapter::GetBounds()
{
  return {0, 0, 0, 0};
}

void WindowAdapter::SetTitle(std::string title)
{
}

std::string WindowAdapter::GetTitle()
{
  return {};
}

void WindowAdapter::SetOpacity(float opacity)
{
}

float WindowAdapter::GetOpacity()
{
  return 1.0f;
}

void WindowAdapter::SetUndecorated(bool undecorate)
{
}

bool WindowAdapter::IsUndecorated()
{
  return false;
}

void WindowAdapter::SetIcon(jgui::Image *image)
{
}

jgui::Image * WindowAdapter::GetIcon()
{
  return nullptr;
}

void WindowAdapter::Repaint(Component *cmp)
{
}

void WindowAdapter::SetVisible(bool visible)
{
}

bool WindowAdapter::IsVisible()
{
  return false;
}
    
jwindow_rotation_t WindowAdapter::GetRotation()
{
  return JWR_NONE;
}

void WindowAdapter::SetRotation(jwindow_rotation_t t)
{
}

void WindowAdapter::SetCursorLocation(int x, int y)
{
}

jpoint_t<int> WindowAdapter::GetCursorLocation()
{
  return {0, 0};
}

void WindowAdapter::SetCursorEnabled(bool enable)
{
}

bool WindowAdapter::IsCursorEnabled()
{
  return false;
}

void WindowAdapter::SetCursor(jcursor_style_t t)
{
}

void WindowAdapter::SetCursor(jgui::Image *shape, int hotx, int hoty)
{
}

}

