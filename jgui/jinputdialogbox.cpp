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
#include "jinputdialogbox.h"

namespace jgui {

InputDialogBox::InputDialogBox(std::string title, std::string msg, int x, int y):
   		jgui::Frame(title, x, y, 1000, 1)
{
	jcommon::Object::SetClassName("jgui::InputDialogBox");

	int cw = DEFAULT_COMPONENT_WIDTH,
			ch = DEFAULT_COMPONENT_HEIGHT;

	_label = new Label(msg, _insets.left, _insets.top, _size.width, _size.height);

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetHorizontalAlign(JUSTIFY_HALIGN);
	_label->SetVerticalAlign(TOP_VALIGN);

	_label->SetSize(_label->GetPreferredSize());

	_field = new TextField(_label->GetX(), _label->GetY()+_label->GetHeight()+10, _label->GetWidth(), 40);

	_ok = new Button("Ok", _label->GetX()+_label->GetWidth()-2*cw-1*30, _field->GetY()+_field->GetHeight()+20, cw, ch);
	_cancel = new Button("Cancel", _label->GetX()+_label->GetWidth()-1*cw-0*30, _field->GetY()+_field->GetHeight()+20, cw, ch);
	
	_field->SetNavigation(NULL, NULL, NULL, _ok);
	_ok->SetNavigation(NULL, _cancel, _field, _cancel);
	_cancel->SetNavigation(_ok, NULL, _field, NULL);

	_ok->RegisterButtonListener(this);
	_cancel->RegisterButtonListener(this);
	
	Add(_label);
	Add(_field);
	Add(_ok);
	Add(_cancel);

	_field->RequestFocus();

	Pack();
}

InputDialogBox::~InputDialogBox() 
{
		jthread::AutoLock lock(&_input_mutex);

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
		jthread::AutoLock lock(&_input_mutex);

		Hide();

		_frame_sem.Notify();
}

}
