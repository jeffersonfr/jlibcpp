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

  _label.SetText(msg);

  _label.SetWrap(true);
  _label.SetHorizontalAlign(JHA_LEFT);

  _yes.RegisterActionListener(this);
  _no.RegisterActionListener(this);
  
  _buttons_layout.SetAlign(jgui::JFLA_RIGHT);
  _buttons_container.SetLayout(&_buttons_layout);

  _buttons_container.Add(&_yes);
  _buttons_container.Add(&_no);

  _buttons_container.SetPreferredSize(_buttons_layout.GetPreferredLayoutSize(&_buttons_container));

  Add(&_label, jgui::JBLA_CENTER);
  Add(&_buttons_container, jgui::JBLA_SOUTH);

  _no.RequestFocus();
}

YesNoDialog::~YesNoDialog() 
{
}

void YesNoDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
  _label.SetHorizontalAlign(align);
}

jhorizontal_align_t YesNoDialog::GetHorizontalAlign()
{
  return _label.GetHorizontalAlign();
}

void YesNoDialog::SetVerticalAlign(jvertical_align_t align)
{
  _label.SetVerticalAlign(align);
}

jvertical_align_t YesNoDialog::GetVerticalAlign()
{
  return _label.GetVerticalAlign();
}

void YesNoDialog::ActionPerformed(jevent::ActionEvent *event)
{
  std::string response = "no";

  if (GetFocusOwner() == &_yes) {
    response = "yes";
  }
    
  GetParams()->SetTextParam("response", response);

  DispatchDataEvent(new jevent::DataEvent(this, GetParams()));

  Close();
}

}
