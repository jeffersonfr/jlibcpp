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
#include "jinputdialogbox.h"
#include "jdataevent.h"

namespace jgui {

InputDialogBox::InputDialogBox(std::string title, std::string msg):
	jgui::Widget(title, -1, -1, -1, -1)
{
	jcommon::Object::SetClassName("jgui::InputDialogBox");

	int cw = DEFAULT_COMPONENT_WIDTH,
			ch = DEFAULT_COMPONENT_HEIGHT;

	jgui::jinsets_t insets = GetInsets();

	_label = new Label(msg, insets.left, insets.top, _size.width-insets.left-insets.right, _size.height-insets.top-insets.bottom);

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetHorizontalAlign(JHA_JUSTIFY);
	_label->SetVerticalAlign(JVA_TOP);

	_label->SetSize(_label->GetPreferredSize());

	_field = new TextField(_label->GetX(), _label->GetY()+_label->GetHeight()+10, _label->GetWidth(), 40);

	_ok = new Button("Ok", _label->GetX()+_label->GetWidth()-2*cw-1*30, _field->GetY()+_field->GetHeight()+20, cw, ch);
	_cancel = new Button("Cancel", _label->GetX()+_label->GetWidth()-1*cw-0*30, _field->GetY()+_field->GetHeight()+20, cw, ch);
	
	_ok->RegisterActionListener(this);
	_cancel->RegisterActionListener(this);
	
	Add(_label);
	Add(_field);
	Add(_ok);
	Add(_cancel);

	_field->RequestFocus();

	Pack(true);
}

InputDialogBox::~InputDialogBox() 
{
		delete _label;
		delete _field;
		delete _ok;
		delete _cancel;
}

std::string InputDialogBox::GetText()
{
	if (_field != NULL) {
		return _field->GetText();
	}

	return "";
}

void InputDialogBox::SetHorizontalAlign(jhorizontal_align_t align)
{
	_label->SetHorizontalAlign(align);
}

jhorizontal_align_t InputDialogBox::GetHorizontalAlign()
{
	return _label->GetHorizontalAlign();
}

void InputDialogBox::SetVerticalAlign(jvertical_align_t align)
{
	_label->SetVerticalAlign(align);
}

jvertical_align_t InputDialogBox::GetVerticalAlign()
{
	return _label->GetVerticalAlign();
}

void InputDialogBox::ActionPerformed(jgui::ButtonEvent *event)
{
	GetParams()->SetTextParam("text", _field->GetText());

	DispatchDataEvent(new jcommon::DataEvent(this, GetParams()));

	// Release();
}

}
