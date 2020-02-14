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
#include "jgui/jyesnodialog.h"
#include "jevent/jdataevent.h"

namespace jgui {

YesNoDialog::YesNoDialog(Container *parent, std::string title, std::string msg):
  jgui::Dialog(title, parent, {0, 0, 560, 280})
{
  jcommon::Object::SetClassName("jgui::YesNoDialog");

  jgui::jsize_t
    size = GetSize();
  jgui::jinsets_t 
    insets = GetInsets();

  _label = new Label(msg, {insets.left, insets.top, size.width - insets.left - insets.right, 180});

  _label->SetWrap(true);
  _label->SetHorizontalAlign(JHA_JUSTIFY);
  _label->SetVerticalAlign(JVA_TOP);

  _no = new Button("No", {size.width - insets.right - DEFAULT_COMPONENT_WIDTH, insets.top + 180 + 8, DEFAULT_COMPONENT_WIDTH, DEFAULT_COMPONENT_HEIGHT});
  _yes = new Button("Yes", {size.width - insets.right - 2*DEFAULT_COMPONENT_WIDTH - 8, insets.top + 180 + 8, DEFAULT_COMPONENT_WIDTH, DEFAULT_COMPONENT_HEIGHT});
  
  _yes->RegisterActionListener(this);
  _no->RegisterActionListener(this);
  
  Add(_label);
  Add(_yes);
  Add(_no);

  _no->RequestFocus();
  
  Pack(false);
}

YesNoDialog::~YesNoDialog() 
{
  if (_label != nullptr) {
    delete _label;
    _label = nullptr;
  }

  if (_yes != nullptr) {
    delete _yes;
    _yes = nullptr;
  }

  if (_no != nullptr) {
    delete _no;
    _no = nullptr;
  }
}

void YesNoDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label->SetHorizontalAlign(align);
}

jhorizontal_align_t YesNoDialog::GetHorizontalAlign()
{
  return _label->GetHorizontalAlign();
}

void YesNoDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label->SetVerticalAlign(align);
}

jvertical_align_t YesNoDialog::GetVerticalAlign()
{
  return _label->GetVerticalAlign();
}

void YesNoDialog::ActionPerformed(jevent::ActionEvent *event)
{
  std::string response = "no";

  if (GetFocusOwner() == _yes) {
    response = "yes";
  }
    
  GetParams()->SetTextParam("response", response);

  DispatchDataEvent(new jevent::DataEvent(this, GetParams()));

  Close();
}

}
