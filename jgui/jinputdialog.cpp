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
#include "jgui/jinputdialog.h"
#include "jevent/jdataevent.h"

namespace jgui {

InputDialog::InputDialog(Container *parent, std::string title, std::string msg):
  jgui::Dialog(title, parent, {0, 0, 560, 280})
{
  jcommon::Object::SetClassName("jgui::InputDialog");

  jgui::jsize_t
    size = GetSize();
  jgui::jinsets_t 
    insets = GetInsets();

  _label = new Label(msg, {insets.left, insets.top, size.width - insets.left - insets.right, 180});

  _label->SetWrap(true);
  _label->SetHorizontalAlign(JHA_JUSTIFY);
  _label->SetVerticalAlign(JVA_TOP);

  _field = new TextField({insets.left, insets.top + 180 + 8, size.width - insets.left - insets.right, DEFAULT_COMPONENT_HEIGHT});
  _cancel = new Button("Cancel", {size.width - insets.right - DEFAULT_COMPONENT_WIDTH, insets.top + 180 + 8 + DEFAULT_COMPONENT_HEIGHT + 8, DEFAULT_COMPONENT_WIDTH, DEFAULT_COMPONENT_HEIGHT});
  _ok = new Button("Ok", {size.width - insets.right - 2*DEFAULT_COMPONENT_WIDTH - 8, insets.top + 180 + 8 + DEFAULT_COMPONENT_HEIGHT + 8, DEFAULT_COMPONENT_WIDTH, DEFAULT_COMPONENT_HEIGHT});
  
  _ok->RegisterActionListener(this);
  _cancel->RegisterActionListener(this);
  
  Add(_label);
  Add(_field);
  Add(_ok);
  Add(_cancel);

  _cancel->RequestFocus();
  
  Pack(false);
}

InputDialog::~InputDialog() 
{
  if (_label != nullptr) {
    delete _label;
    _label = nullptr;
  }

  if (_field != nullptr) {
    delete _field;
    _field = nullptr;
  }

  if (_ok != nullptr) {
    delete _ok;
    _ok = nullptr;
  }

  if (_cancel != nullptr) {
    delete _cancel;
    _cancel = nullptr;
  }
}

std::string InputDialog::GetText()
{
  if (_field != NULL) {
    return _field->GetText();
  }

  return "";
}

void InputDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label->SetHorizontalAlign(align);
}

jhorizontal_align_t InputDialog::GetHorizontalAlign()
{
  return _label->GetHorizontalAlign();
}

void InputDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label->SetVerticalAlign(align);
}

jvertical_align_t InputDialog::GetVerticalAlign()
{
  return _label->GetVerticalAlign();
}

void InputDialog::ActionPerformed(jevent::ActionEvent *event)
{
  GetParams()->SetTextParam("text", _field->GetText());

  DispatchDataEvent(new jevent::DataEvent(this, GetParams()));

  Close();
}

}
