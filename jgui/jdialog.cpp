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
#include "jgui/jdialog.h"
#include "jgui/jwindow.h"
#include "jexception/jinvalidargumentexception.h"
#include "jexception/jnullpointerexception.h"

namespace jgui {

Dialog::Dialog(Container *parent, int x, int y, int width, int height):
	jgui::Container(x, y, width, height)
{
  if (parent == nullptr) {
    throw jexception::NullPointerException("Parent must be a valid pointer");
  }

  if (dynamic_cast<jgui::Window *>(parent) == nullptr) {
    throw jexception::InvalidArgumentException("Parent must be a window");
  }

  SetParent(parent);

  _focus_owner = nullptr;
  _is_modal = false;
}

Dialog::~Dialog()
{
  Close();
}

void Dialog::Paint(Graphics *g)
{
  Container::Paint(g);
}

bool Dialog::IsModal()
{
  return _is_modal;
}

void Dialog::Exec(bool modal)
{
  GetParent()->InternalAddDialog(this);
}

void Dialog::Close()
{
  GetParent()->InternalRemoveDialog(this);
}

jgui::Component * Dialog::GetFocusOwner()
{
  return _focus_owner;
}

void Dialog::RequestComponentFocus(jgui::Component *c)
{
  if (_focus_owner != nullptr && _focus_owner != c) {
    _focus_owner->ReleaseFocus();
  }

  _focus_owner = c;

  Repaint();

  dynamic_cast<Component *>(c)->DispatchFocusEvent(new jevent::FocusEvent(c, jevent::JFET_GAINED));
}

void Dialog::ReleaseComponentFocus(jgui::Component *c)
{
  if (_focus_owner == nullptr or _focus_owner != c) {
    return;
  }

  _focus_owner = nullptr;

  Repaint();

  dynamic_cast<Component *>(c)->DispatchFocusEvent(new jevent::FocusEvent(c, jevent::JFET_LOST));
}

Container * Dialog::GetFocusCycleRootAncestor()
{
  return this;
}

bool Dialog::KeyPressed(jevent::KeyEvent *event)
{
  Container::KeyPressed(event);

  if (event->GetSymbol() == jevent::JKS_ESCAPE) {
    Close();
  }

  return true;
}

bool Dialog::KeyReleased(jevent::KeyEvent *event)
{
  Container::KeyReleased(event);

  return true;
}

bool Dialog::KeyTyped(jevent::KeyEvent *event)
{
  Container::KeyTyped(event);

  return true;
}

bool Dialog::MousePressed(jevent::MouseEvent *event)
{
  Container::MousePressed(event);

  return true;
}

bool Dialog::MouseReleased(jevent::MouseEvent *event)
{
  Container::MouseReleased(event);

  return true;
}

bool Dialog::MouseMoved(jevent::MouseEvent *event)
{
  Container::MouseMoved(event);

  return true;
}

bool Dialog::MouseWheel(jevent::MouseEvent *event)
{
  Container::MouseWheel(event);

  return true;
}

}
