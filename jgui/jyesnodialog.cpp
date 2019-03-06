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
	jgui::Dialog(parent)
{
	jcommon::Object::SetClassName("jgui::YesNoDialog");

	int 
    cw = DEFAULT_COMPONENT_WIDTH,
		ch = DEFAULT_COMPONENT_HEIGHT;

  jgui::jsize_t
    size = GetSize();
  jgui::jinsets_t
    insets = GetInsets();

	_label = new Label(msg, insets.left, insets.top, size.width - insets.left - insets.right, size.height - insets.top - insets.bottom);

	// _label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetHorizontalAlign(JHA_JUSTIFY);
	_label->SetVerticalAlign(JVA_TOP);

	_label->SetSize(_label->GetPreferredSize());

  jgui::jregion_t
    t = _label->GetVisibleBounds();

	_yes = new Button("Sim", t.x + t.width - 2*cw - 1*30, t.y + t.height + 20, cw, ch);
	_no = new Button("Nao", t.x + t.width - 1*cw - 0*30, t.y + t.height + 20, cw, ch);
	
	_no->RegisterActionListener(this);
	_yes->RegisterActionListener(this);

	Add(_label);
	Add(_no);
	Add(_yes);

	_no->RequestFocus();

	Pack(true);
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
