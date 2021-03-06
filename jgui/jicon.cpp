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
#include "jgui/jicon.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

Icon::Icon(jgui::Image *image):
  Component()
{
  jcommon::Object::SetClassName("jgui::Icon");

  _halign = JHA_CENTER;
  _valign = JVA_CENTER;

  _image = image;
}

Icon::~Icon()
{
  if (_image != nullptr) {
    delete _image;
    _image = nullptr;
  }
}

void Icon::SetImage(jgui::Image *image)
{
  if (_image != nullptr) {
    delete _image;
    _image = nullptr;
  }

  _image = image;

  Repaint();
}

jgui::Image * Icon::GetImage()
{
  return _image;
}

void Icon::SetHorizontalAlign(jhorizontal_align_t align)
{
  if (_halign != align) {
    _halign = align;

    Repaint();
  }
}

jhorizontal_align_t Icon::GetHorizontalAlign()
{
  return _halign;
}

void Icon::SetVerticalAlign(jvertical_align_t align)
{
  if (_valign != align) {
    _valign = align;

    Repaint();
  }
}

jvertical_align_t Icon::GetVerticalAlign()
{
  return _valign;
}
    
void Icon::Paint(Graphics *g)
{
  JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jgui::jsize_t
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  int
    w = size.width - GetHorizontalPadding(),
    h = size.height - GetVerticalPadding();

  if (_image != nullptr) {
    // TODO:: add contains, cover, ....
    g->DrawImage(_image, {GetPadding().left, GetPadding().top, w, h});
  }
}

}
