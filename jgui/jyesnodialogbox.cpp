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
#include "Stdafx.h"
#include "jyesnodialogbox.h"

namespace jgui {

YesNoDialogBox::YesNoDialogBox(std::string title, std::string msg):
	jgui::DialogBox(title, -1, -1, -1, -1)
{
	jcommon::Object::SetClassName("jgui::YesNoDialogBox");

	int cw = DEFAULT_COMPONENT_WIDTH,
			ch = DEFAULT_COMPONENT_HEIGHT;

	_label = new Label(msg, _insets.left, _insets.top, _size.width-_insets.left-_insets.right, _size.height-_insets.top-_insets.bottom);

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetHorizontalAlign(JHA_JUSTIFY);
	_label->SetVerticalAlign(JVA_TOP);

	_label->SetSize(_label->GetPreferredSize());

	_yes = new Button("Sim", _label->GetX()+_label->GetWidth()-2*cw-1*30, _label->GetY()+_label->GetHeight()+20, cw, ch);
	_no = new Button("Nao", _label->GetX()+_label->GetWidth()-1*cw-0*30, _label->GetY()+_label->GetHeight()+20, cw, ch);
	
	_no->RegisterButtonListener(this);
	_yes->RegisterButtonListener(this);

	Add(_label);
	Add(_no);
	Add(_yes);

	_no->RequestFocus();

	Pack();
}

YesNoDialogBox::~YesNoDialogBox() 
{
	delete _label;
	delete _yes;
	delete _no;
}

void YesNoDialogBox::SetHorizontalAlign(jhorizontal_align_t align)
{
	_label->SetHorizontalAlign(align);
}

jhorizontal_align_t YesNoDialogBox::GetHorizontalAlign()
{
	return _label->GetHorizontalAlign();
}

void YesNoDialogBox::SetVerticalAlign(jvertical_align_t align)
{
	_label->SetVerticalAlign(align);
}

jvertical_align_t YesNoDialogBox::GetVerticalAlign()
{
	return _label->GetVerticalAlign();
}

void YesNoDialogBox::ActionPerformed(jgui::ButtonEvent *event)
{
	std::string response = "no";

	if (GetFocusOwner() == _yes) {
		response = "yes";
	}
		
	GetParams()->SetTextParam("response", response);

	DispatchDataEvent(GetParams());

	Release();
}

}
