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
#include "jgui/jmessagedialog.h"

namespace jgui {

MessageDialog::MessageDialog(Container *parent, std::string title, std::string msg):
  jgui::Dialog(title, parent, {0, 0, 560, 280})
{
  jcommon::Object::SetClassName("jgui::MessageDialog");

  jgui::jsize_t
    size = GetSize();
  jgui::jinsets_t 
    insets = GetInsets();

  _label = new Label(msg, {insets.left, insets.top, size.width - insets.left - insets.right, 180});

  _label->SetWrap(true);
  _label->SetHorizontalAlign(JHA_JUSTIFY);
  _label->SetVerticalAlign(JVA_TOP);

  _ok = new Button("Ok", {size.width - insets.right - DEFAULT_COMPONENT_WIDTH, insets.top + 180 + 8 + 8, DEFAULT_COMPONENT_WIDTH, DEFAULT_COMPONENT_HEIGHT});
  
  _ok->RegisterActionListener(this);
  
  Add(_label);
  Add(_ok);

  _ok->RequestFocus();

  Pack(false);
}

MessageDialog::~MessageDialog() 
{
  if (_label != nullptr) {
    delete _label;
    _label = nullptr;
  }

  if (_ok != nullptr) {
    delete _ok;
    _ok = nullptr;
  }
}

void MessageDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label->SetHorizontalAlign(align);
}

jhorizontal_align_t MessageDialog::GetHorizontalAlign()
{
  return _label->GetHorizontalAlign();
}

void MessageDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label->SetVerticalAlign(align);
}

jvertical_align_t MessageDialog::GetVerticalAlign()
{
  return _label->GetVerticalAlign();
}

void MessageDialog::ActionPerformed(jevent::ActionEvent *event)
{
  Close();
}

}
