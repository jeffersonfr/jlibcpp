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

	int lines = Component::CountLines(msg, _size.width-_insets.left-_insets.right-20, _font);

	if (lines <= 0) {
		lines = 1;
	}

	_label = new Label(msg, _insets.left, _insets.top, _size.width-_insets.left-_insets.right, (lines)*_font->GetHeight()+10);

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetTruncated(false);

	_field = new TextField(_insets.left, _label->GetY()+_label->GetHeight()+10, _size.width-_insets.left-_insets.right, (int)(_font->GetHeight()));

	_ok = new Button("Ok", _size.width-(2*200+60), _field->GetY()+_field->GetHeight()+20, 200, 40);
	_cancel = new Button("Cancel", _size.width-(1*200+60)+30, _field->GetY()+_field->GetHeight()+20, 200, 40);
	
	_field->SetNavigation(NULL, NULL, NULL, _ok);
	_ok->SetNavigation(NULL, _cancel, _field, _cancel);
	_cancel->SetNavigation(_ok, NULL, _field, NULL);

	_ok->SetAlign(CENTER_ALIGN);
	_cancel->SetAlign(CENTER_ALIGN);

	_ok->RegisterButtonListener(this);
	_cancel->RegisterButtonListener(this);
	
	Add(_label);
	Add(_field);
	Add(_ok);
	Add(_cancel);

	_field->RequestFocus();

	// SetSize(1000, (_font->GetHeight())*(lines)+100);

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

void InputDialogBox::ActionPerformed(jgui::ButtonEvent *event)
{
		jthread::AutoLock lock(&_input_mutex);

		Hide();

		_frame_sem.Notify();
}

}
